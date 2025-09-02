#ifndef __STOPPABLE_TASK_0C4D_HPP
#define __STOPPABLE_TASK_0C4D_HPP

#include "Routine.hpp"

#include <mutex>
#include <thread>

namespace Stoppable {
/**
 * @brief Convenience wrapper for Stoppable::Routine
 *
 */
struct Task {
  Task(const Routine::Cycle& cycle, const Routine::ExceptionHandler& handler);

  ///@cond
  Task(const Task&) = delete;
  Task(Task&& other) = delete;
  Task& operator=(const Task&) = delete;
  Task& operator=(Task&& other) = delete;
  ///@endcond

  /**
   * @brief Destroy the Task object, calls Task::stop() if the task is still
   * running
   *
   */
  ~Task();

  /**
   * @brief Start running the Stoppable::Routine::run() and wait for the first
   * Routine::Cycle call
   *
   */
  void start() noexcept;

  /**
   * @brief Check if the task is running
   *
   * @return true if Task::start() was called
   * @return false if Task::start() was never called or if Task::stop() was
   */
  bool running() const noexcept;

  /**
   * @brief Stop the Stoppable::Routine::run() and wait for the last
   * Routine::Cycle call
   *
   */
  void stop() noexcept;

private:
  std::mutex mx_;
  StopTokenPtr token_ = makeStopToken();
  Routine::ExceptionHandler handler_;
  std::shared_ptr<Routine> routine_;
  std::unique_ptr<std::thread> routine_thread_;
};

using TaskPtr = std::shared_ptr<Task>;
} // namespace Stoppable

#endif //__STOPPABLE_TASK_0C4D_HPP