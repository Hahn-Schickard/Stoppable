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
    routine_thread_ = make_unique<thread>([routine_ptr = weak_ptr(routine_)]() {
      if (auto routine = routine_ptr.lock()) {
        routine->run();
      }
    });
    is_running.wait();
  }
}

bool Task::running() const noexcept {
  if (routine_thread_) {
    return routine_thread_->joinable();
  } else {
    return false;
  }
}

void Task::stop() noexcept {
  if (running()) {
    unique_lock guard(mx_);
    token_->stop();
    try {
      routine_thread_->join();
    } catch (...) {
      handler_(current_exception());
    }
  }
}
} // namespace Stoppable
