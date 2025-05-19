#ifndef __MULTITHREADING_STOPABLE_HPP
#define __MULTITHREADING_STOPABLE_HPP

#include <chrono>
#include <functional>
#include <future>
#include <memory>

namespace Stoppable{
/**
 * @brief Defines a Stoppable routine
 *
 */
struct Stoppable {
  using Iteration = std::function<void()>;

  explicit Stoppable(Iteration&& iteration) 
      : exitFuture_(exitSignal_.get_future()), iteration_(std::move(iteration)) {}

  Stoppable(Stoppable&& instance)
      : exitSignal_(std::move(instance.exitSignal_)),
        exitFuture_(std::move(instance.exitFuture_)) {}

  Stoppable& operator=(Stoppable&& instance) {
    exitFuture_ = std::move(instance.exitFuture_);
    exitSignal_ = std::move(instance.exitSignal_);
    return *this;
  }

  virtual ~Stoppable() = default;

  /**
   * @brief Starts to run a given routine
   *
   */
  void start() {
    do {
      iteration_();
    } while (!stopRequested());
  }

  /**
   * @brief Checks if exit signal was set
   *
   * @return true
   * @return false
   */
  bool stopRequested() {
    return exitFuture_.wait_for(std::chrono::milliseconds(0)) ==
            std::future_status::timeout
        ? false
        : true;
  }

  /**
   * @brief Sets the exit signal, so the next iteration of the run routine will
   * not be executed
   *
   */
  void stop() { exitSignal_.set_value(); }

private:
  std::promise<void> exitSignal_;
  std::future<void> exitFuture_;
  Iteration iteration_;
};

using StoppablePtr = std::shared_ptr<Stoppable>;
}
#endif //__MULTITHREADING_STOPABLE_HPP