#include "StoppableTask.hpp"

#include "gtest/gtest.h"
#include <memory>
#include <string>

using namespace std;

class TaskTests : public ::testing::Test {
protected:
  void SetUp() override {
    task_ = new StoppableTask([](){}, "Task");
  }
  void TearDown() override { delete task_; }

  StoppableTask* task_;
};

TEST_F(TaskTests, canStartTask) {
  EXPECT_NO_THROW(task_->start());
}

TEST_F(TaskTests, canStopTask) {
  EXPECT_NO_THROW(task_->start());
  EXPECT_NO_THROW(task_->stop());
}

TEST_F(TaskTests, returnsFalseWhenTaskIsRunning) {
  EXPECT_NO_THROW(task_->start());
  EXPECT_FALSE(task_->start());
}

TEST_F(TaskTests, returnsFalseWhenTaskIsStopped) {
  EXPECT_NO_THROW(task_->start());
  EXPECT_NO_THROW(task_->stop());
  EXPECT_FALSE(task_->stop());
}
