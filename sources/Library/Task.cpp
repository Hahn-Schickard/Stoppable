#include "Task.hpp"

namespace Stoppable {

Task::Task(
    const Routine::Cycle& cycle, const Routine::ExceptionHandler& handler)
    : handler_(handler),
      routine_(std::make_shared<Routine>(token_, cycle, handler_)) {}

Task::~Task() { stop(); }

void Task::start() noexcept {
  if (!running()) {
    std::unique_lock guard(mx_);
    token_->reset();
    auto is_running = routine_->running();
    routine_finished_ = std::async(
        std::launch::async, [routine_ptr = std::weak_ptr(routine_)]() {
          if (auto routine = routine_ptr.lock()) {
            routine->run();
          }
        });
    is_running.wait();
  }
}

bool Task::running() const noexcept {
  try {
    using namespace std::chrono_literals;
    return routine_finished_.wait_for(10ms) == std::future_status::timeout;
  } catch (const std::future_error&) {
    return false; // no future, thus not running
  }
}

void Task::stop() noexcept {
  if (running()) {
    std::unique_lock guard(mx_);
    token_->stop();
    try {
      routine_finished_.get();
    } catch (...) {
      handler_(std::current_exception());
    }
  }
}
} // namespace Stoppable
