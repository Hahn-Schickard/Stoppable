#ifndef __MULTITHREADING_JOBS_HANDLER_HPP
#define __MULTITHREADING_JOBS_HANDLER_HPP

#include "Stoppable.hpp"

#include <chrono>
#include <deque>
#include <future>
#include <mutex>

struct JobHandler : public Stoppable {
  using ExceptionHandler = std::function<void(const std::exception &)>;

  JobHandler(ExceptionHandler handler,
             std::chrono::microseconds timeout = std::chrono::microseconds(10))
      : Stoppable(), handler_(handler), clear_timeout_(timeout) {}

  void add(std::future<void> &&job) {
    std::lock_guard expansion_lock(
        jobs_mutex_); // lock it so cleaner does not un erase something
    jobs_.push_back(std::move(job));
  }

  template <typename Job, typename... Args>
  void emplace(Job &&job, Args &&... args) {
    std::lock_guard expansion_lock(
        jobs_mutex_); // lock it so cleaner does not un erase something
    jobs_.emplace_back(std::forward<Job>(job), std::forward < Args(args)...);
  }

private:
  void tryClean() {
    for (auto it = jobs_.begin(); it < jobs_.end(); it++) {
      auto status = it->wait_for(clear_timeout_);
      if (status == std::future_status::ready) {
        std::lock_guard deletion_lock(
            jobs_mutex_); // lock it so notify, does not expand vector size,
        // moving the end iterator position
        try {
          it->get(); // cleanup the allocated memory
        } catch (std::exception &ex) {
          handler_(std::move(ex));
        }
        jobs_.erase(it);
      }
    }
  }

  void clean() {
    for (auto it = jobs_.begin(); it < jobs_.end(); it++) {
      try {
        it->get(); // this will block until job is completed, might need to
                   // add cancelation flag to EventListener's handleEvent
                   // method as well
      } catch (std::exception &ex) {
        handler_(std::move(ex));
      }
      jobs_.erase(it);
    }
  }

  virtual void run() override {
    while (!stopRequested()) {
      // maybe wait for a conditional_variable?
      if (jobs_.empty()) {
        std::this_thread::sleep_for(clear_timeout_);
      } else {
        tryClean();
      }
    }
    clean();
  }

  ExceptionHandler handler_;
  std::chrono::microseconds clear_timeout_;
  std::deque<std::future<void>> jobs_;
  std::mutex jobs_mutex_;
};

#endif //__MULTITHREADING_JOBS_HANDLER_HPP