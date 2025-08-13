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
  using Iteration = std::function<void()>;

  explicit Routine(const Iteration& iteration)
      : exit_future_(exit_signal_.get_future()), iteration_(iteration) {}

  Routine(Routine&& instance)
      : exit_signal_(std::move(instance.exit_signal_)),
        exit_future_(std::move(instance.exit_future_)) {}

  Routine& operator=(Routine&& instance) {
    exit_signal_ = std::move(instance.exit_signal_);
    exit_future_ = std::move(instance.exit_future_);
    return *this;
  }

  virtual ~Routine() = default;

  void start() {
    do {
      iteration_();
    } while (!stopRequested());
  }

  bool stopRequested() const {
    using namespace std::chrono;
    return !(exit_future_.wait_for(0ms) == std::future_status::timeout);
  }

  void stop() { exit_signal_.set_value(); }

private:
  std::promise<void> exit_signal_;
  std::future<void> exit_future_;
  Iteration iteration_;
};

using RoutinePtr = std::shared_ptr<Routine>;
} // namespace Stoppable
#endif //__MULTITHREADING_STOPABLE_HPP