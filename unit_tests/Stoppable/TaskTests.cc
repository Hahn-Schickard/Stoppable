#include "Task.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

namespace Stoppable::tests {
using namespace std;
using namespace ::testing;

// NOLINTNEXTLINE(readability-identifier-naming)
MATCHER_P(ExceptionPointee, exception_type, "") {
  try {
    rethrow_exception(arg);
    return false;
  } catch (const exception& ex) {
    return typeid(ex) == typeid(exception_type);
  }
}

class TaskTests : public Test {
protected:
  TaskTests()
      : task_(make_unique<Task>(
            mock_cycle_.AsStdFunction(), mock_handler_.AsStdFunction())) {}

  MockFunction<void()> mock_cycle_;
  MockFunction<void(const std::exception_ptr&)> mock_handler_;
  unique_ptr<Task> task_;
};

TEST_F(TaskTests, canStartStop) {
  EXPECT_CALL(mock_handler_, Call(_)).Times(Exactly(0));
  EXPECT_CALL(mock_cycle_, Call()).Times(AtLeast(1));

  EXPECT_FALSE(task_->running());
  EXPECT_NO_THROW(task_->start());
  EXPECT_TRUE(task_->running());
  EXPECT_NO_THROW(task_->stop());
  EXPECT_FALSE(task_->running());
}

TEST_F(TaskTests, canRestart) {
  EXPECT_CALL(mock_handler_, Call(_)).Times(Exactly(0));
  EXPECT_CALL(mock_cycle_, Call()).Times(AtLeast(2));

  EXPECT_FALSE(task_->running());
  EXPECT_NO_THROW(task_->start());
  EXPECT_TRUE(task_->running());
  EXPECT_NO_THROW(task_->stop());
  EXPECT_FALSE(task_->running());
  EXPECT_FALSE(task_->running());
  EXPECT_NO_THROW(task_->start());
  EXPECT_TRUE(task_->running());
  EXPECT_NO_THROW(task_->stop());
  EXPECT_FALSE(task_->running());
}

TEST_F(TaskTests, canHandleException) {
  auto target_exception = logic_error("Test thrown exceptions");

  EXPECT_CALL(mock_handler_, Call(ExceptionPointee(target_exception)))
      .Times(Exactly(1));
  EXPECT_CALL(mock_cycle_, Call())
      .Times(AtLeast(1))
      .WillOnce(Throw(target_exception));

  EXPECT_FALSE(task_->running());
  EXPECT_NO_THROW(task_->start());
  EXPECT_TRUE(task_->running());
  EXPECT_NO_THROW(task_->stop());
}

} // namespace Stoppable::tests