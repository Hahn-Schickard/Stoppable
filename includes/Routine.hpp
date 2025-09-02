#ifndef __STOPPABLE_ROUTINE_FD0C_HPP
#define __STOPPABLE_ROUTINE_FD0C_HPP

#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <stdexcept>

namespace Stoppable {
/**
 * @brief Used to stop the Routine::run() loop
 *
 */
struct StopToken {
  /**
   * @brief Sets the internal flag to false
   *
   */
  void reset() noexcept;

  /**
   * @brief Sets the internal flag to true
   *
   */
  void stop() noexcept;

  /**
   * @brief Returns the current state of the internal flag
   *
   * @return true if stop() was called
   * @return false if stop() was never called or if reset() was
   */
  bool stopping() const noexcept;

private:
  std::atomic<bool> flag_ = false;
};

using StopTokenPtr = std::shared_ptr<StopToken>;

StopTokenPtr makeStopToken();

/**
 * @brief A stoppable operation that is continuously called in a loop until stop
 * token is set
 *
 */
struct Routine {
  /**
   * @brief Callable operation that can be stopped
   *
   * @attention Calling this object, MUST NOT infinitely block
   *
   */
  using Cycle = std::function<void()>;

  /**
   * @brief Handles any exceptions thrown from the Cycle callable operation
   *
   */
  using ExceptionHandler = std::function<void(const std::exception_ptr&)>;

  Routine(const StopTokenPtr& stop_token, const Cycle& cycle,
      const ExceptionHandler& handler);

  ///@cond
  Routine(const Routine&) = delete;
  Routine(Routine&& other) = delete;
  Routine& operator=(const Routine&) = delete;
  Routine& operator=(Routine&& other) = delete;
  ///@endcond

  /**
   * @brief Destroy the Routine object, sets the managed stop token
   *
   */
  ~Routine();

  /**
   * @brief Run the Cycle callable in a loop until StopToken::stop() is called
   *
   */
  void run() noexcept;

  /**
   * @brief Assigns a future object to inform users when initial Routine::run()
   * loop is executed
   *
   * The resulting future will be set, once first Cycle call is made
   *
   * @return std::future<void>
   */
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