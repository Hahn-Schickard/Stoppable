#include "JobHandler.hpp"
#include "StoppableTask.hpp"

#include "gtest/gtest.h"
#include <chrono>
#include <exception>
#include <functional>
#include <future>
#include <memory>
#include <string>
#include <thread>

using namespace std;

void rethrowException(exception_ptr thrown) {
  if (thrown) {
    rethrow_exception(thrown);
  }
}

template <typename EXCEPTION_TYPE> void expectException(exception_ptr thrown) {
  EXPECT_THROW(rethrow_exception(thrown), EXCEPTION_TYPE);
}

struct FakeException : runtime_error {
  FakeException() : runtime_error("Fake Exception") {}
};

struct FakeJob {
  FakeJob() = default;

  FakeJob(chrono::milliseconds delay) : delay_(delay) {}

  void operator()(shared_ptr<bool> completed) {
    this_thread::sleep_for(delay_);
    *completed = true;
  }

  void operator()() {
    this_thread::sleep_for(delay_);
    throw FakeException();
  }

private:
  chrono::milliseconds delay_ = chrono::milliseconds(0);
};

TEST(JobHandlerTests, canAddAndClearJob) {
  try {
    auto exception_handler = std::bind(&rethrowException, placeholders::_1);
    auto job_handler = make_shared<JobHandler>(move(exception_handler));
    auto job_handler_task =
        make_unique<StoppableTask>(job_handler, "Job Handler");

    job_handler_task->startTask();

    auto job_completed = make_shared<bool>(false);

    auto job_handled_future =
        async(launch::async, FakeJob(chrono::milliseconds(10)), job_completed);
    job_handler->add(move(job_handled_future));
    this_thread::sleep_for(chrono::milliseconds(20));

    EXPECT_TRUE(*job_completed);
  } catch (exception &ex) {
    FAIL() << "Caught an unhandled exception: " << ex.what() << endl;
  }
}

TEST(JobHandlerTests, canEmplaceAndClearJob) {
  try {
    auto exception_handler = std::bind(&rethrowException, placeholders::_1);
    auto job_handler = make_shared<JobHandler>(move(exception_handler));
    auto job_handler_task =
        make_unique<StoppableTask>(job_handler, "Job Handler");

    job_handler_task->startTask();

    auto job_completed = make_shared<bool>(false);

    job_handler->emplace(
        async(launch::async, FakeJob(chrono::milliseconds(10)), job_completed));
    this_thread::sleep_for(chrono::milliseconds(20));

    EXPECT_TRUE(*job_completed);
  } catch (exception &ex) {
    FAIL() << "Caught an unhandled exception: " << ex.what() << endl;
  }
}

TEST(JobHandlerTests, canCleanOnStop) {
  try {
    auto exception_handler = std::bind(&rethrowException, placeholders::_1);
    auto job_handler = make_shared<JobHandler>(move(exception_handler));
    auto job_handler_task =
        make_unique<StoppableTask>(job_handler, "Job Handler");

    job_handler_task->startTask();

    auto job_completed = make_shared<bool>(false);

    auto job_handled_future =
        async(launch::async, FakeJob(chrono::milliseconds(10)), job_completed);
    job_handler->add(move(job_handled_future));
    job_handler_task->stopTask();

    EXPECT_TRUE(*job_completed);
  } catch (exception &ex) {
    FAIL() << "Caught an unhandled exception: " << ex.what() << endl;
  }
}

TEST(JobHandlerTests, canCleanExceptionOnStop) {
  try {
    auto exception_handler =
        std::bind(&expectException<FakeException>, placeholders::_1);
    auto job_handler = make_shared<JobHandler>(move(exception_handler));
    auto job_handler_task =
        make_unique<StoppableTask>(job_handler, "Job Handler");

    job_handler_task->startTask();

    auto job_handled_future =
        async(launch::async, FakeJob(chrono::milliseconds(10)));
    job_handler->add(move(job_handled_future));
    job_handler_task->stopTask();
  } catch (exception &ex) {
    FAIL() << "Caught an unhandled exception: " << ex.what() << endl;
  }
}

TEST(JobHandlerTests, canHandleException) {
  try {
    auto exception_handler =
        std::bind(&expectException<FakeException>, placeholders::_1);
    auto job_handler = make_shared<JobHandler>(move(exception_handler));
    auto job_handler_task =
        make_unique<StoppableTask>(job_handler, "Job Handler");

    job_handler_task->startTask();

    auto job_handled_future =
        async(launch::async, FakeJob(chrono::milliseconds(10)));
    job_handler->add(move(job_handled_future));
    this_thread::sleep_for(chrono::milliseconds(20));
  } catch (exception &ex) {
    FAIL() << "Caught an unhandled exception: " << ex.what() << endl;
  }
}

TEST(JobHandlerTests, canHandleExceptionAndStartNewJob) {
  try {
    auto exception_handler =
        std::bind(&expectException<FakeException>, placeholders::_1);
    auto job_handler = make_shared<JobHandler>(move(exception_handler));
    auto job_handler_task =
        make_unique<StoppableTask>(job_handler, "Job Handler");

    job_handler_task->startTask();

    auto throws_exception =
        async(launch::async, FakeJob(chrono::milliseconds(10)));
    job_handler->add(move(throws_exception));
    auto job_completed = make_shared<bool>(false);
    auto completes_job =
        async(launch::async, FakeJob(chrono::milliseconds(10)), job_completed);
    job_handler->add(move(completes_job));

    this_thread::sleep_for(chrono::milliseconds(20));

    EXPECT_TRUE(*job_completed);
  } catch (exception &ex) {
    FAIL() << "Caught an unhandled exception: " << ex.what() << endl;
  }
}