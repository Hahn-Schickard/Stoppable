#ifndef __MULTITHREADING_STOPABLE_TASK_HPP
#define __MULTITHREADING_STOPABLE_TASK_HPP

#include "Stoppable.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

struct StoppableTaskIsAlreadyRunning : public std::runtime_error {
  StoppableTaskIsAlreadyRunning(std::string const &message)
      : std::runtime_error(message) {}
};

struct StoppableTaskIsNotRunning : public std::runtime_error {
  StoppableTaskIsNotRunning(std::string const &message)
      : std::runtime_error(message) {}
};

class StoppableTask {
  std::unique_ptr<Stoppable> task_;
  std::string task_name_;
  std::unique_ptr<std::thread> task_thread_;

public:
  /**
   * @brief Construct a new empty Stoppable Task object
   *
   */
  StoppableTask() : StoppableTask(std::unique_ptr<Stoppable>(), "") {}
  /**
   * @brief Construct a new Stoppable Task object with a given Stoppable thread
   * and thread name
   *
   * @param task
   * @param task_name
   */
  StoppableTask(std::unique_ptr<Stoppable> task, std::string task_name)
      : task_(std::move(task)), task_name_(task_name),
        task_thread_(std::make_unique<std::thread>()) {}

  /**
   * @brief Starts the task, throws StoppableTaskIsAlreadyRunning if the task is
   * already running
   *
   */
  void startTask() {
    if (!task_thread_->joinable())
      task_thread_ = std::make_unique<std::thread>([&]() { task_->start(); });
    else {
      std::string error_msg = "Task" + task_name_ + " is already running";
      throw StoppableTaskIsAlreadyRunning(std::move(error_msg));
    }
  }

  /**
   * @brief Stops the task, throws StoppableTaskIsNotRunning if the task is
   * already stopped
   *
   */
  void stopTask() {
    if (task_thread_->joinable()) {
      task_->stop();
      task_thread_->join();
    } else {
      std::string error_msg = "Task" + task_name_ + " is not running";
      throw StoppableTaskIsNotRunning(std::move(error_msg));
    }
  }

  /**
   * @brief Returns the name of this task
   *
   * @return std::string
   */
  std::string getName() { return task_name_; }
};

#endif //__MULTITHREADING_STOPABLE_TASK_HPP