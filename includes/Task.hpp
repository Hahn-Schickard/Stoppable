#ifndef __MULTITHREADING_STOPABLE_routine_HPP
#define __MULTITHREADING_STOPABLE_routine_HPP

#include "Routine.hpp"

#include <mutex>
#include <stdexcept>

namespace Stoppable {
/**
 * @brief Defines a Stoppable task which is run in a separate thread
 *
 */

struct Task {
  Task(const Routine::Cycle& cycle, const Routine::ExceptionHandler& handler)
      : cycle_(cycle), handler_(handler) {}

  Task(const Task&) = delete;
  Task(Task&& other) = delete;
  Task& operator=(const Task&) = delete;
  Task& operator=(Task&& other) = delete;

  virtual ~Task() { stop(); }

  void start() noexcept {
    if (!running()) {
      std::unique_lock guard(mx_);
      routine_ = std::make_unique<Routine>(cycle_, handler_);
      auto is_running = routine_->running();
      routine_finished_ =
          std::async(std::launch::async, [this]() { routine_->run(); });
      is_running.wait();
    }
  }

  bool running() noexcept {
    try {
      using namespace std::chrono;
      std::unique_lock guard(mx_);
      return routine_finished_.wait_for(10ms) == std::future_status::timeout;
    } catch (const std::future_error&) {
      return false; // no future, thus not running
    }
  }

  void stop() noexcept {
    if (running()) {
      std::unique_lock guard(mx_);
      routine_->stop();
      routine_.reset();
      routine_finished_.wait();
    }
  }

private:
  std::mutex mx_;
  Routine::Cycle cycle_;
  Routine::ExceptionHandler handler_;
  std::unique_ptr<Routine> routine_;
  std::future<void> routine_finished_;
};

using TaskPtr = std::unique_ptr<Task>;
} // namespace Stoppable

#endif //__MULTITHREADING_STOPABLE_routine_HPP