#include "StoppableTask.hpp"

#include "gtest/gtest.h"
#include <memory>
#include <string>

using namespace std;

class StoppableFake : public Stoppable {
  void run() override {
    do {
      // do nothing
    } while (!stopRequested());
  }
};

class StoppableTaskTests : public ::testing::Test {
protected:
  void SetUp() override {
    task_ = new StoppableTask(make_unique<StoppableFake>(), "Task");
  }
  void TearDown() override { delete task_; }

  StoppableTask* task_;
};

// NOLINTNEXTLINE
TEST_F(StoppableTaskTests, canReturnTaskName) {
  string expected = "Task";
  EXPECT_EQ(task_->getName(), expected);
}

// NOLINTNEXTLINE
TEST_F(StoppableTaskTests, canStartTask) {
  EXPECT_NO_THROW(task_->startTask());
}

// NOLINTNEXTLINE
TEST_F(StoppableTaskTests, canStopTask) {
  EXPECT_NO_THROW(task_->startTask());
  EXPECT_NO_THROW(task_->stopTask());
}

// NOLINTNEXTLINE
TEST_F(StoppableTaskTests, returnsFalseWhenTaskIsRunning) {
  EXPECT_NO_THROW(task_->startTask());
  EXPECT_FALSE(task_->startTask());
}

// NOLINTNEXTLINE
TEST_F(StoppableTaskTests, returnsFalseWhenTaskIsStopped) {
  EXPECT_NO_THROW(task_->startTask());
  EXPECT_NO_THROW(task_->stopTask());
  EXPECT_FALSE(task_->stopTask());
}
