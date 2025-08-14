#ifndef __STOPPABLE_ROUTINE_FD0C_HPP
#define __STOPPABLE_ROUTINE_FD0C_HPP

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <shared_mutex>
#include <stdexcept>

namespace Stoppable {
struct StopToken {

  void reset() { flag_ = false; }

  void stop() { flag_ = true; }

  bool stopping() { return flag_; }

private:
  std::atomic<bool> flag_ = false;
};

using StopTokenPtr = std::shared_ptr<StopToken>;

struct Routine {
  using Cycle = std::function<void()>;
  using ExceptionHandler = std::function<void(const std::exception_ptr&)>;

  Routine(const StopTokenPtr& stop_token, const Cycle& cycle,
      const ExceptionHandler& handler)
      : stop_token_(stop_token), cycle_(cycle), handler_(handler) {}

  Routine(const Routine&) = delete;
  Routine(Routine&& other) = delete;
  Routine& operator=(const Routine&) = delete;
  Routine& operator=(Routine&& other) = delete;

  ~Routine() { stop_token_->stop(); }

  void run() noexcept {
    running_.set_value();
    do {
      try {
        cycle_();
      } catch (...) {
        handler_(std::current_exception());
      }
    } while (!stop_token_->stopping());
    running_ = {}; // reset promise for a re-run
  }

  std::future<void> running() noexcept { return running_.get_future(); }

private:
  std::promise<void> running_;
  StopTokenPtr stop_token_;
  Cycle cycle_;
  ExceptionHandler handler_;
};

using RoutinePtr = std::unique_ptr<Routine>;
} // namespace Stoppable
#endif //__STOPPABLE_ROUTINE_FD0C_HPP