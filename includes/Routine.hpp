#ifndef __MULTITHREADING_STOPABLE_HPP
#define __MULTITHREADING_STOPABLE_HPP

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <memory>

namespace Stoppable {
/**
 * @brief Defines a Stoppable routine
 *
 */
struct Routine {
  using Cycle = std::function<void()>;
  using ExceptionHandler = std::function<void(const std::exception_ptr&)>;

  explicit Routine(const Cycle& cycle, const ExceptionHandler& handler)
      : exited_(exit_.get_future()), stopped_(stop_.get_future()),
        cycle_(cycle), handler_(handler) {}

  Routine(const Routine&) = delete;
  Routine(Routine&& other) = delete;
  Routine& operator=(const Routine&) = delete;
  Routine& operator=(Routine&& other) = delete;

  ~Routine() { stop(); }

  void run() noexcept {
    running_.set_value();
    do {
      try {
        cycle_();
      } catch (...) {
        handler_(std::current_exception());
      }
    } while (!stopRequested());
    stop_.set_value();
  }

  std::future<void> running() noexcept { return running_.get_future(); }

  bool stopRequested() const {
    using namespace std::chrono;
    return exited_.wait_for(0ms) != std::future_status::timeout;
  }

  void stop() noexcept {
    try {
      exit_.set_value();
      stopped_.wait();
    } catch (const std::future_error&) {
      // do nothing, routine already stopped
    }
  }

private:
  std::promise<void> running_;
  std::promise<void> exit_;
  std::future<void> exited_;
  std::promise<void> stop_;
  std::future<void> stopped_;
  Cycle cycle_;
  ExceptionHandler handler_;
};

using RoutinePtr = std::unique_ptr<Routine>;
} // namespace Stoppable
#endif //__MULTITHREADING_STOPABLE_HPP