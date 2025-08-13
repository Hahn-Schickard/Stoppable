#include "Routine.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <thread>

namespace Stoppable::tests {
using namespace std;
using namespace std::chrono;
using namespace ::testing;

TEST(RoutineTests, canRun) {
  MockFunction<void()> mock_cycle;
  MockFunction<void(const std::exception_ptr&)> mock_handler;

  EXPECT_CALL(mock_handler, Call(_)).Times(Exactly(0));
  EXPECT_CALL(mock_cycle, Call()).Times(AtLeast(1));

  auto routine = make_unique<Routine>(
      mock_cycle.AsStdFunction(), mock_handler.AsStdFunction());
  auto is_running = routine->running();
  auto routine_thread = thread([&routine]() { routine->run(); });

  EXPECT_NE(is_running.wait_for(100ms), future_status::timeout);
  routine.reset();
  routine_thread.join();
}
} // namespace Stoppable::tests