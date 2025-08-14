#ifndef __STOPPABLE_ROUTINE_FD0C_HPP
#define __STOPPABLE_ROUTINE_FD0C_HPP

#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <stdexcept>

namespace Stoppable {
struct StopToken {

  void reset() noexcept;

  void stop() noexcept;

  bool stopping() const noexcept;

private:
  std::atomic<bool> flag_ = false;
};

using StopTokenPtr = std::shared_ptr<StopToken>;

StopTokenPtr makeStopToken();

struct Routine {
  using Cycle = std::function<void()>;
  using ExceptionHandler = std::function<void(const std::exception_ptr&)>;

  Routine(const StopTokenPtr& stop_token, const Cycle& cycle,
      const ExceptionHandler& handler);

  Routine(const Routine&) = delete;
  Routine(Routine&& other) = delete;
  Routine& operator=(const Routine&) = delete;
  Routine& operator=(Routine&& other) = delete;

  ~Routine();

  void run() noexcept;

  std::future<void> running() noexcept;

private:
  std::promise<void> running_;
  StopTokenPtr stop_token_;
  Cycle cycle_;
  ExceptionHandler handler_;
};

using RoutinePtr = std::shared_ptr<Routine>;
} // namespace Stoppable
#endif //__STOPPABLE_ROUTINE_FD0C_HPP