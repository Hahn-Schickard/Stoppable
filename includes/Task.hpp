#ifndef __MULTITHREADING_STOPABLE_routine_HPP
#define __MULTITHREADING_STOPABLE_routine_HPP

#include "Routine.hpp"

#include <mutex>
#include <stdexcept>
#include <thread>

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
    if (!routine_thread_) {
      std::unique_lock guard(mx_);
      routine_ = std::make_unique<Routine>(cycle_, handler_);
      auto is_running = routine_->running();
      routine_thread_ =
          std::make_unique<std::thread>([this]() { routine_->run(); });
      is_running.wait();
    }
  }

  bool running() noexcept {
    std::unique_lock guard(mx_);
    return routine_thread_ != nullptr;
  }

  void stop() noexcept {
    if (routine_thread_) {
      std::unique_lock guard(mx_);
      if (routine_thread_->joinable()) {
        routine_->stop();
        routine_.reset();
        routine_thread_->join();
      }
      routine_thread_.reset();
    }
  }

private:
  std::mutex mx_;
  Routine::Cycle cycle_;
  Routine::ExceptionHandler handler_;
  std::unique_ptr<Routine> routine_;
  std::unique_ptr<std::thread> routine_thread_;
};

using TaskPtr = std::unique_ptr<Task>;
} // namespace Stoppable

#endif //__MULTITHREADING_STOPABLE_routine_HPP