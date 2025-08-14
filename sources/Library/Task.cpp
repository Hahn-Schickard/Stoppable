#include "Task.hpp"

namespace Stoppable {
using namespace std;

Task::Task(
    const Routine::Cycle& cycle, const Routine::ExceptionHandler& handler)
    : token_(makeStopToken()), handler_(handler),
      routine_(make_shared<Routine>(token_, cycle, handler_)) {}

Task::~Task() { stop(); }

void Task::start() noexcept {
  if (!running()) {
    unique_lock guard(mx_);
    token_->reset();
    auto is_running = routine_->running();
    routine_finished_ =
        async(launch::async, [routine_ptr = weak_ptr(routine_)]() {
          if (auto routine = routine_ptr.lock()) {
            routine->run();
          }
        });
    is_running.wait();
  }
}

bool Task::running() const noexcept {
  try {
    using namespace chrono_literals;
    return routine_finished_.wait_for(10ms) == future_status::timeout;
  } catch (const future_error&) {
    return false; // no future, thus not running
  }
}

void Task::stop() noexcept {
  if (running()) {
    unique_lock guard(mx_);
    token_->stop();
    try {
      routine_finished_.get();
    } catch (...) {
      handler_(current_exception());
    }
  }
}
} // namespace Stoppable
