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
    task = new StoppableTask(make_unique<StoppableFake>(), "Task");
  }
  void TearDown() override {
    try {
      task->stopTask();
    } catch (StoppableTaskIsNotRunning &ex) {
      // do nothing
    }
    delete task;
  }

  StoppableTask *task;
};

TEST_F(StoppableTaskTests, canReturnTaskName) {
  string expected = "Task";
  EXPECT_EQ(task->getName(), expected);
}

TEST_F(StoppableTaskTests, canStartTask) { EXPECT_NO_THROW(task->startTask()); }

TEST_F(StoppableTaskTests, canStopTask) {
  EXPECT_NO_THROW(task->startTask());
  EXPECT_NO_THROW(task->stopTask());
}

TEST_F(StoppableTaskTests, throwsStoppableTaskIsAlreadyRunning) {
  EXPECT_NO_THROW(task->startTask());
  EXPECT_THROW(task->startTask(), StoppableTaskIsAlreadyRunning);
}

TEST_F(StoppableTaskTests, throwsStoppableTaskIsNotRunning) {
  EXPECT_NO_THROW(task->startTask());
  EXPECT_NO_THROW(task->stopTask());
  EXPECT_THROW(task->stopTask(), StoppableTaskIsNotRunning);
}