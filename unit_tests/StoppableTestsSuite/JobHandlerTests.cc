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

void handleException(exception_ptr thrown) {
  try {
    if (thrown) {
      rethrow_exception(thrown);
    }
  } catch (const exception &e) {
    cerr << "Caught exception: " << e.what() << endl;
  }
}

template <typename EXCEPTION_TYPE> void expectException(exception_ptr thrown) {
  EXPECT_THROW(rethrow_exception(thrown), EXCEPTION_TYPE);
}

struct FakeJob {
  FakeJob() = default;

  FakeJob(chrono::milliseconds delay) : delay_(delay) {}

  void operator()() { this_thread::sleep_for(delay_); }

private:
  chrono::milliseconds delay_ = chrono::milliseconds(0);
};

TEST(JobHandlerTests, tmp) {
  auto exception_handler = std::bind(&handleException, placeholders::_1);
  auto job_handler = make_unique<JobHandler>(move(exception_handler));

  auto job_handled_future =
      async(launch::async, FakeJob(chrono::milliseconds(10)));
  job_handler->add(move(job_handled_future));
  this_thread::sleep_for(chrono::milliseconds(20));
}