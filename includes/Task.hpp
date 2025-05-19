#ifndef __MULTITHREADING_STOPABLE_TASK_HPP
#define __MULTITHREADING_STOPABLE_TASK_HPP

#include "Stoppable.hpp"

#include <memory>
#include <stdexcept>
#include <thread>

namespace Stoppable {
/**
 * @brief Defines a Stoppable task which is run in a separate thread
 *
 */
struct Task {
  using ExceptionHandler = std::function<void(const std::exception_ptr&)>;

  Task(const ExceptionHandler& handler, Stoppable::Iteration&& iteration)
      : exception_handler_(handler),
        task_(std::make_unique<Stoppable>(std::move(iteration))) {}

  virtual ~Task() {
    try {
      stop();
    } catch (...) {
      exception_handler_(std::current_exception());
    }
  }

  bool start() {
    if (!task_thread_) {
      task_thread_ = std::make_unique<std::thread>([this]() {
        try {
          task_->start();
        } catch (...) {
          exception_handler_(std::current_exception());
        }
      });
      return task_thread_->joinable();
    }
    return false;
  }

  bool stop() {
    if (task_thread_) {
      if (task_thread_->joinable()) {
        task_->stop();
        task_thread_->join();
        return !task_thread_->joinable();
      }
    }
    return false;
  }

private:
  ExceptionHandler exception_handler_;
  std::unique_ptr<Stoppable> task_;
  std::unique_ptr<std::thread> task_thread_;
};
} // namespace Stoppable

#endif //__MULTITHREADING_STOPABLE_TASK_HPP