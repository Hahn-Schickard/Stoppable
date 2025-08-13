#ifndef __MULTITHREADING_STOPABLE_routine_HPP
#define __MULTITHREADING_STOPABLE_routine_HPP

#include "Routine.hpp"

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

  Task(Routine&& routine, const ExceptionHandler& handler)
      : routine_(std::make_unique<Routine>(std::move(routine))),
        handler_(handler) {}

  Task(const Routine::Iteration& iteration, const ExceptionHandler& handler)
      : routine_(std::make_unique<Routine>(iteration)), handler_(handler) {}

  virtual ~Task() {
    try {
      stop();
    } catch (...) {
      handler_(std::current_exception());
    }
  }

  void start() {
    if (!routine_thread_) {
      routine_thread_ = std::make_unique<std::thread>([this]() {
        try {
          routine_->start();
        } catch (...) {
          handler_(std::current_exception());
        }
      });
    }
  }

  void stop() {
    if (routine_thread_) {
      if (routine_thread_->joinable()) {
        routine_->stop();
        routine_thread_->join();
      }
    }
  }

private:
  std::unique_ptr<Routine> routine_;
  ExceptionHandler handler_;
  std::unique_ptr<std::thread> routine_thread_;
};
} // namespace Stoppable

#endif //__MULTITHREADING_STOPABLE_routine_HPP