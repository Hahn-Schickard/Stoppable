#ifndef __MULTITHREADING_JOBS_HANDLER_HPP
#define __MULTITHREADING_JOBS_HANDLER_HPP

#include "Stoppable.hpp"

#include <chrono>
#include <future>
#include <list>
#include <mutex>
#include <thread>

struct JobHandler : public Stoppable {
  using ExceptionHandler = std::function<void(std::exception_ptr)>;

  JobHandler(ExceptionHandler handler,
      std::chrono::microseconds timeout = std::chrono::microseconds(10))
      : Stoppable(), handler_(handler), clear_timeout_(timeout) {}

  void add(std::future<void>&& job) {
    std::lock_guard expansion_lock(
        jobs_mutex_); // lock it so cleaner does not un erase something
    jobs_.push_back(std::move(job));
  }

  template <typename Job, typename... Args>
  void emplace(Job&& job, Args&&... args) {
    std::lock_guard expansion_lock(
        jobs_mutex_); // lock it so cleaner does not un erase something
    jobs_.emplace_back(std::forward<Job>(job), std::forward<Args>(args)...);
  }

private:
  void tryClean() {
    jobs_.remove_if([&](auto& it) {
      auto status = it.wait_for(clear_timeout_);
      if (status == std::future_status::ready) {
        std::lock_guard deletion_lock(
            jobs_mutex_); // lock it so notify, does not expand vector size,
        // moving the end iterator position
        try {
          it.get(); // cleanup the allocated memory
        } catch (...) {
          handler_(std::current_exception());
        }
        return true;
      } else {
        return false;
      }
    });
  }

  void clean() {
    std::lock_guard deletion_lock(jobs_mutex_);
    for (auto it = jobs_.begin(); it != jobs_.end();) {
      try {
        it->get();
      } catch (...) {
        handler_(std::current_exception());
      }
      it = jobs_.erase(it);
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
  std::list<std::future<void>> jobs_;
  std::mutex jobs_mutex_;
};

using JobHandlerPtr = std::shared_ptr<JobHandler>;

#endif //__MULTITHREADING_JOBS_HANDLER_HPP