# Introduction
## Stoppable
### Brief description

A header only implementation of a utility classes that help develop multi-threaded code, requires std::thread library and C++11 support.

### Contains:
 * `Stoppable` - A class that provides functionality to start and stop a given routine
 * `StoppableTask` - A class that manages `Stoppable` in a separate thread
 * `JobHandler` - A class that cleans up allocated `std::future` instances from `std::async` calls.

### Example
```cpp
#include "Stoppable/JobHandler.hpp"
#include "Stoppable/StoppableTask.hpp"

#include <functional>
#include <iostream>
#include <thread>

using namespace std;

class StoppableImplementation : public Stoppable {
  void run() override {
    do {
      cout << "Running cycle!" << endl;
      this_thread::sleep_for(1s);
    } while (!stopRequested());
  }
};

void handleException(const std::exception_ptr &ex_ptr) {
  try {
    if (ex_ptr) {
      rethrow_exception(ex_ptr);
    }
  } catch (const exception &e) {
    cout << e.what() << endl;
  }
}

int main() {
  {
    auto task = make_unique<StoppableTask>(
        make_shared<StoppableImplementation>(), "Runner");
    cout << "Starting " << task->getName() << endl;
    task->startTask();
    this_thread::sleep_for(0.5s);
    task->stopTask();
    cout << task->getName() << " stopped" << endl;
  }

  cout << "All Tasks have bean cleaned up!" << endl;

  {
    auto job_handler =
        make_unique<JobHandler>(bind(&handleException, placeholders::_1));
    job_handler->add(async(launch::async, [&]() {
      cout << "Job Started" << endl;
      this_thread::sleep_for(0.5s);
      cout << "Job finished" << endl;
    }));
  }
  cout << "All Jobs have bean cleaned up!" << endl;

  exit(EXIT_SUCCESS);
}
```