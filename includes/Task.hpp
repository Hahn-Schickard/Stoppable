#ifndef __STOPPABLE_TASK_0C4D_HPP
#define __STOPPABLE_TASK_0C4D_HPP

#include "Routine.hpp"

namespace Stoppable {
struct Task {
  Task(const Routine::Cycle& cycle, const Routine::ExceptionHandler& handler);

  Task(const Task&) = delete;
  Task(Task&& other) = delete;
  Task& operator=(const Task&) = delete;
  Task& operator=(Task&& other) = delete;

  ~Task();

  void start() noexcept;

  bool running() const noexcept;

  void stop() noexcept;

private:
  std::mutex mx_;
  StopTokenPtr token_;
  Routine::ExceptionHandler handler_;
  std::shared_ptr<Routine> routine_;
  std::future<void> routine_finished_;
};

using TaskPtr = std::shared_ptr<Task>;
} // namespace Stoppable

#endif //__STOPPABLE_TASK_0C4D_HPP