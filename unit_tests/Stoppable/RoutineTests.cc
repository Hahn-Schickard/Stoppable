#include "ExceptionPointee.hpp"
#include "Routine.hpp"

#include <string>
#include <thread>

namespace Stoppable::tests {
using namespace std;
using namespace std::chrono_literals;
using namespace ::testing;

class RoutineTests : public Test {
protected:
  thread testCanRun() {
    auto is_running = routine_->running();
    auto routine_thread = thread([routine_ptr = weak_ptr(routine_)]() {
      if (auto routine = routine_ptr.lock()) {
        routine->run();
      }
    });
    EXPECT_NE(is_running.wait_for(100ms), future_status::timeout);
    return routine_thread;
  }

  void testCanRunAndStop() {
    auto test_thread = testCanRun();
    EXPECT_FALSE(token_->stopping());
    token_->stop();
    EXPECT_TRUE(token_->stopping());
    test_thread.join();
  }

  MockFunction<void()> mock_cycle_;
  MockFunction<void(const exception_ptr&)> mock_handler_;
  StopTokenPtr token_ = make_shared<StopToken>();
  RoutinePtr routine_ = make_shared<Routine>(
      token_, mock_cycle_.AsStdFunction(), mock_handler_.AsStdFunction());
};

TEST_F(RoutineTests, canRunAndStop) {
  EXPECT_CALL(mock_handler_, Call(_)).Times(Exactly(0));
  EXPECT_CALL(mock_cycle_, Call()).Times(AtLeast(1));

  EXPECT_NO_FATAL_FAILURE(testCanRunAndStop());
}

TEST_F(RoutineTests, canRunAndStopWithoutFuture) {
  EXPECT_CALL(mock_handler_, Call(_)).Times(Exactly(0));
  EXPECT_CALL(mock_cycle_, Call()).Times(AtLeast(1));

  auto test_thread = thread([routine_ptr = weak_ptr(routine_)]() {
    if (auto routine = routine_ptr.lock()) {
      routine->run();
    }
  });
  this_thread::sleep_for(100ms);

  EXPECT_FALSE(token_->stopping());
  token_->stop();
  EXPECT_TRUE(token_->stopping());
  test_thread.join();
}

TEST_F(RoutineTests, canRestart) {
  EXPECT_CALL(mock_handler_, Call(_)).Times(Exactly(0));
  EXPECT_CALL(mock_cycle_, Call()).Times(AtLeast(2));

  EXPECT_NO_FATAL_FAILURE(testCanRunAndStop());
  token_->reset();
  EXPECT_NO_FATAL_FAILURE(testCanRunAndStop());
}

TEST_F(RoutineTests, canThrowAndRun) {
  auto target_exception = logic_error("Test thrown exceptions");

  promise<void> mock_cycle_call;
  EXPECT_CALL(mock_handler_, Call(_)).Times(Exactly(0));
  EXPECT_CALL(mock_handler_, Call(ExceptionPointee(target_exception)))
      .Times(Exactly(1));
  EXPECT_CALL(mock_cycle_, Call())
      .Times(AtLeast(2))
      .WillOnce(Throw(target_exception))
      .WillOnce([&mock_cycle_call]() { mock_cycle_call.set_value(); })
      .WillRepeatedly([]() {
        // default action for valgrind, not normally used in the test
        // valgrind causes test execution to take longer, thus causing more
        // cycle calls. If no default action is given, gtest will return
        // directly, but this causes significant slow down in valgrind execution
        this_thread::sleep_for(100ms);
      });

  auto mock_cycle_called = mock_cycle_call.get_future();

  auto test_thread = testCanRun();
  mock_cycle_called.get();
  token_->stop();
  test_thread.join();

  if (HasFatalFailure()) {
    FAIL();
  }
}

TEST_F(RoutineTests, canThrowAndStop) {
  auto target_exception = logic_error("Test stopping on thrown exceptions");

  EXPECT_CALL(mock_handler_, Call(ExceptionPointee(target_exception)))
      .Times(Exactly(1))
      .WillOnce([this]() { token_->stop(); });
  EXPECT_CALL(mock_cycle_, Call())
      .Times(Exactly(1))
      .WillOnce(Throw(target_exception));

  auto test_thread = testCanRun();
  test_thread.join();

  if (HasFatalFailure()) {
    FAIL();
  }
}
} // namespace Stoppable::tests