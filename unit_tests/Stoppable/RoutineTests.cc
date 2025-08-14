#include "Routine.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <thread>

#include <iostream>

namespace Stoppable::tests {
using namespace std;
using namespace std::chrono;
using namespace ::testing;

TEST(RoutineTests, canRun) {
  MockFunction<void()> mock_cycle;
  MockFunction<void(const std::exception_ptr&)> mock_handler;

  EXPECT_CALL(mock_handler, Call(_)).Times(Exactly(0));
  EXPECT_CALL(mock_cycle, Call()).Times(AtLeast(1));

  auto token = std::make_shared<StopToken>();
  cout << "Making routine" << endl;
  auto routine =
      Routine(token, mock_cycle.AsStdFunction(), mock_handler.AsStdFunction());
  cout << "Starting routine" << endl;
  auto is_running = routine.running();
  auto routine_thread = thread([&routine]() { routine.run(); });
  cout << "Routine started" << endl;
  EXPECT_NE(is_running.wait_for(100ms), future_status::timeout);
  cout << "Stopping routine" << endl;
  token->stop();
  cout << "Routine stopped" << endl;
  routine_thread.join();
  cout << "Thread cleaned up" << endl;
}
} // namespace Stoppable::tests