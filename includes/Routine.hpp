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

  explicit Routine(const Cycle& cycle)
      : exited_(exit_.get_future()), cycle_(cycle) {}

  // Routine(const Routine&) = delete;
  // Routine(Routine&& other) = delete;
  // Routine& operator=(const Routine&) = delete;
  // Routine& operator=(Routine&& other) = delete;

  virtual ~Routine() { exit_.set_value(); }

  void run() {
    running_.set_value();
    do {
      cycle_();
    } while (!stopRequested());
  }

  std::future<void> running() { return running_.get_future(); }

  bool stopRequested() const {
    using namespace std::chrono;
    return !(exited_.wait_for(0ms) == std::future_status::timeout);
  }

private:
  std::promise<void> running_;
  std::promise<void> exit_;
  std::future<void> exited_;
  Cycle cycle_;
};

using RoutinePtr = std::unique_ptr<Routine>;
} // namespace Stoppable
#endif //__MULTITHREADING_STOPABLE_HPP