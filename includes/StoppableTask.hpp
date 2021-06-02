#ifndef __MULTITHREADING_STOPABLE_TASK_HPP
#define __MULTITHREADING_STOPABLE_TASK_HPP

#include "Stoppable.hpp"

#include <memory>
#include <string>
#include <thread>

/**
 * @brief Defines a Stoppable task which is run in a separate thread
 *
 */
class StoppableTask {
  StoppablePtr task_;
  std::string task_name_;
  std::unique_ptr<std::thread> task_thread_;

public:
  /**
   * @brief Construct a new empty Stoppable Task object
   *
   */
  StoppableTask() : StoppableTask(StoppablePtr(), "") {}
  /**
   * @brief Construct a new Stoppable Task object with a given Stoppable routine
   * and task name
   *
   * @param task
   * @param task_name
   */
  StoppableTask(StoppablePtr task, std::string task_name)
      : task_(std::move(task)), task_name_(task_name),
        task_thread_(std::make_unique<std::thread>()) {}

  ~StoppableTask() { stopTask(); }

  /**
   * @brief Starts the task, throws StoppableTaskIsAlreadyRunning if the task is
   * already running
   *
   */
  bool startTask() {
    if (!task_thread_->joinable()) {
      task_thread_ = std::make_unique<std::thread>(
          [](StoppablePtr task) { task->start(); }, task_);
      return task_thread_->joinable();
    } else {
      return false;
    }
  }

  /**
   * @brief Stops the task, throws StoppableTaskIsNotRunning if the task is
   * already stopped
   *
   */
  bool stopTask() {
    if (task_thread_->joinable()) {
      task_->stop();
      task_thread_->join();
      return !task_thread_->joinable();
    } else {
      return false;
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