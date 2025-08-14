#include "Routine.hpp"

namespace Stoppable {
using namespace std;

void StopToken::reset() noexcept { flag_ = false; }

void StopToken::stop() noexcept { flag_ = true; }

bool StopToken::stopping() const noexcept { return flag_; }

StopTokenPtr makeStopToken() { return make_shared<StopToken>(); }

Routine::Routine(const StopTokenPtr& stop_token, const Routine::Cycle& cycle,
    const Routine::ExceptionHandler& handler)
    : stop_token_(stop_token), cycle_(cycle), handler_(handler) {}

Routine::~Routine() { stop_token_->stop(); }

void Routine::run() noexcept {
  running_.set_value();
  do {
    try {
      cycle_();
    } catch (...) {
      handler_(current_exception());
    }
  } while (!stop_token_->stopping());
  running_ = {}; // reset promise for a re-run
}

future<void> Routine::running() noexcept { return running_.get_future(); }

} // namespace Stoppable
