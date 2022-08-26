#ifndef __MULTITHREADING_STOPABLE_HPP
#define __MULTITHREADING_STOPABLE_HPP

#include <chrono>
#include <future>
#include <memory>

/**
 * @brief Defines a Stoppable routine
 *
 */
class Stoppable {
  std::promise<void> exitSignal_;
  std::future<void> exitFuture_;

protected:
  /**
   * @brief Implements a routine that can be stopped
   *
   * Implement in a do-while cycle
   * @code
   * do{
   *  // runner task
   * }while(!stopRequested());
   * @endcode
   *
   */
  virtual void run() = 0;

public:
  Stoppable() : exitFuture_(exitSignal_.get_future()) {}

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
  void start() { run(); }

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
};

using StoppablePtr = std::shared_ptr<Stoppable>;

#endif //__MULTITHREADING_STOPABLE_HPP