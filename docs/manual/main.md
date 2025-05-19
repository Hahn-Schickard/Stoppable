# Introduction
## Stoppable
### Brief description

A header only implementation of a utility classes that help develop multi-threaded code, requires std::thread library and C++11 support.

### Contains:
 * `Stoppable` - A class that provides functionality to start and stop a given routine
 * `Task` - A class that manages `Stoppable` in a separate thread

### Example
```cpp
#include "Stoppable/Task.hpp"

#include <functional>
#include <iostream>
#include <thread>

using namespace std;
using namespace Stoppable;

struct ExampleTask {
  void iteration() {
    cout << "Running cycle " << counter_ << endl;
    counter_++;
    this_thread::sleep_for(10ms);
  }

private:
  size_t counter_ = 0;
};

void handleException(const std::exception_ptr& ex_ptr) {
  try {
    if (ex_ptr) {
      rethrow_exception(ex_ptr);
    }
  } catch (const exception& e) {
    cout << e.what() << endl;
  }
}

int main() {
  {
    auto example_task = make_unique<ExampleTask>();
    auto task = make_unique<Task>(bind(handleException, placeholders::_1),
        bind(&ExampleTask::iteration, *example_task));
    task->start();
    this_thread::sleep_for(0.1s);
    task->stop();
  }

  cout << "All Tasks have bean cleaned up!" << endl;

  exit(EXIT_SUCCESS);
}
```