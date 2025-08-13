#ifndef __MULTITHREADING_STOPABLE_HPP
#define __MULTITHREADING_STOPABLE_HPP

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

  explicit Routine(const Cycle& cycle)
      : exit_future_(exit_signal_.get_future()), cycle_(cycle) {}

  Routine(const Routine&) = delete;

  Routine(Routine&& other)
      : exit_signal_(std::move(other.exit_signal_)),
        exit_future_(std::move(other.exit_future_)) {}

  Routine& operator=(const Routine&) = delete;

  Routine& operator=(Routine&& other) {
    exit_signal_ = std::move(other.exit_signal_);
    exit_future_ = std::move(other.exit_future_);
    return *this;
  }

  virtual ~Routine() { exit_signal_.set_value(); }

  void run() {
    do {
      cycle_();
    } while (!stopRequested());
  }

  bool stopRequested() const {
    using namespace std::chrono;
    return !(exit_future_.wait_for(0ms) == std::future_status::timeout);
  }

private:
  std::promise<void> exit_signal_;
  std::future<void> exit_future_;
  Cycle cycle_;
};

using RoutinePtr = std::shared_ptr<Routine>;
} // namespace Stoppable
#endif //__MULTITHREADING_STOPABLE_HPP