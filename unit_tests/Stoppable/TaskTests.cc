#include "ExceptionPointee.hpp"
#include "Task.hpp"

#include <string>

namespace Stoppable::tests {
using namespace std;
using namespace ::testing;

class TaskTests : public Test {
protected:
  void testCanStartAndStop() {
    EXPECT_FALSE(task_->running());

    EXPECT_NO_THROW(task_->start());
    EXPECT_TRUE(task_->running());

    EXPECT_NO_THROW(task_->stop());
    EXPECT_FALSE(task_->running());
  }

  MockFunction<void()> mock_cycle_;
  MockFunction<void(const std::exception_ptr&)> mock_handler_;
  TaskPtr task_ = make_shared<Task>(
      mock_cycle_.AsStdFunction(), mock_handler_.AsStdFunction());
};

TEST_F(TaskTests, canStartAndStop) {
  EXPECT_CALL(mock_handler_, Call(_)).Times(Exactly(0));
  EXPECT_CALL(mock_cycle_, Call()).Times(AtLeast(1));

  EXPECT_NO_FATAL_FAILURE(testCanStartAndStop());
}

TEST_F(TaskTests, canRestart) {
  EXPECT_CALL(mock_handler_, Call(_)).Times(Exactly(0));
  EXPECT_CALL(mock_cycle_, Call()).Times(AtLeast(2));

  EXPECT_NO_FATAL_FAILURE(testCanStartAndStop());
  // restart task
  EXPECT_NO_FATAL_FAILURE(testCanStartAndStop());
}

TEST_F(TaskTests, canHandleException) {
  auto target_exception = logic_error("Test thrown exceptions");

  EXPECT_CALL(mock_handler_, Call(ExceptionPointee(target_exception)))
      .Times(Exactly(1));
  EXPECT_CALL(mock_cycle_, Call())
      .Times(AtLeast(1))
      .WillOnce(Throw(target_exception));

  EXPECT_NO_FATAL_FAILURE(testCanStartAndStop());
}

} // namespace Stoppable::tests