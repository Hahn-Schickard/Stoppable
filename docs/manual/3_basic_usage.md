# Basic usage

To create any Stoppable::Routine or Stoppable::Task, you will need to provide two callable object that you defined. 

The first one is called the *Cycle* callable. It is a callable object, that does not accept any arguments and does not return anything. This callable is the container for your stoppable operation. Take note, it **MUST NOT** cause indefinite blocking, otherwise you will not be able to stop it. In the bellow given examples, we will use this lambda as a stand in for your *Cycle* callable.

```cpp
auto cycle = []() {
  /*
   * your operation callable. It MUST NOT cause indefinite blocking.
   * This callable will be called in a loop, until the stop token is set
   */
};
```

The second callable, that you **MUST** provide, is called the *ExceptionHandler*. It handles any exceptions that may be thrown from within your Cycle callable. It accepts the `const exception_ptr&` argument, which holds the exception object and returns no values. See [std::exception_ptr](https://en.cppreference.com/w/cpp/error/exception_ptr.html), on how to use it to obtain the exception object. In the bellow given examples, we will use this lambda as a stand in for your *ExceptionHandler* callable.

```cpp
auto handler = [](const exception_ptr&) {
  /* handle any exceptions from the cycle callable */
};
```

## Routine

Stoppable::Routine is the basic wrapper for your stoppable function calls. It uses a Stoppable::StopToken, to stop the function execution. 

In the bellow given example, we will start a separate `std::thread`, which will continually run the `Stoppable::Routine::run()` method until the stop token is set. To ensure that the stop token is not set before a single execution of the `Stoppable::Routine::run()` method was called, we will use the `std::future<void>` object, obtain from `Stoppable::Routine::running()` method, to wait for the first `Stoppable::Routine::run()` execution. Once the stop token is set, we will wait for the routine to finish executing the last Cycle operation by calling the `std::thread::join()` method.

```cpp
#include "Stoppable/Routine.hpp"

#include <thread>

using namespace std;

int main() {
    // create the stop token, this will be used to stop the routine later on
    auto token = Stoppable::makeStopToken();
    // create the routine
    auto routine = Stoppable::Routine(token, cycle, handler);
    // obtain the future object
    auto routine_running = routine.running();
    // start the routine
    auto routine_thread = thread([&routine]() { routine.run(); });
    // wait until routine is started
    routine_running.get();
    // send stop signal to the routine
    token->stop();
    // join the thread, to wait for routine to finish
    routine_thread.join();

  return 0;
}
```

You could also use the `std::async` instead of `std::thread`.

```cpp
#include "Stoppable/Routine.hpp"

#include <future>

using namespace std;

int main() {
    // create the stop token, this will be used to stop the routine later on
    auto token = Stoppable::makeStopToken();
    // create the routine
    auto routine = Stoppable::Routine(token, cycle, handler);
    // obtain the future object
    auto routine_running = routine.running();
    // start the routine
    auto routine_finished =
        async(launch::async, [&routine]() { routine.run(); });
    // wait until routine is started
    routine_running.get();
    // send stop signal to the routine
    token->stop();
    // wait until routine is finished
    routine_finished.get();

  return 0;
}
```

In case you do not want to use `std::async` or `std::thread` to create a separate thread (which can be quite costly), Stoppable::Routine should also work inside a thread-pool. However, since standard C++17 does not provide any ready-made thread-pool implementations, you will have to define one  on your own or use a third-party implementation. In this example `ThreadPool::execute()` is used as a place holder for such thread-pool implementation. Replace these calls with the appropriate thread-pool task assignment calls.

```cpp
#include "Stoppable/Routine.hpp"

using namespace std;

int main() {
    // create the stop token, this will be used to stop the routine later on
    auto token = Stoppable::makeStopToken();
    // create the routine
    auto routine = Stoppable::Routine(token, cycle, handler);
    // start the routine by reference
    ThreadPool::execute([&routine]() { routine.run(); });
    // build and start the routine inside the thread-poll
    ThreadPool::execute([token_ptr = weak_ptr(token), &cycle, &handler]() {
      // weak_ptr is used to avoid shared_ptr memory leaks via lambda capture
      if (auto token_ref = token_ptr.lock()) {
        auto internal_routine = Stoppable::Routine(token_ref, cycle, handler);
        internal_routine.run();
      }
    });
    // send stop signal to the routine
    token->stop();

  return 0;
}
```

### Sharing access to your routines

The Stoppable::Routine does not allow the use of copy or move semantics. This is done to ensure thread-safety without having to design complicate copy and move operations. 

If you need to have shared access to the Stoppable::Routine, you can create it as as `std::shared_ptr`. However, if you do so, you **MUST** pay attention to who owns it and be very careful when passing the `std::shared_ptr` to lambdas via capture block, since lambdas do not decrement the instance counter of the `std::shared_ptr`. Furthermore, you **CAN NOT** rely on the Stoppable::Routine::~Routine() destructor call to automatically set the stop token, since `std::shared_ptr` can live indefinitely inside a lambda.

## Task

Stoppable::Task is a convenience wrapper for Stoppable::Routine. It handles the Stoppable::StopToken, as well as provide a simple way of starting, stopping and restarting Stoppable::Routine.

```cpp
#include "Stoppable/Task.hpp"

using namespace std;

int main() {
  {
    // create a Task
    auto task = Stoppable::Task(cycle, handler);
    // start task execution
    // this will wait until the first Stoppable::Routine::run() was called
    task.start();
    // you can check if the task is running or not
    if (task.running()) {
      //
      task.stop();
    }
    // stopping the task twice does nothing
    task.stop();
    // you can restart tasks that were already stopped
    task.start();
    // starting a task that is already running does nothing
    task.start();
  }
  // tasks stop automatically upon destruction

  return 0;
}
```

### Sharing access to your tasks

Just like the the Stoppable::Routine, Stoppable::Task does not allow the use of copy or move semantics. This is because it uses `std::mutex` inside to protect against simultaneous Stoppable::Task::start() and Stoppable::Task::stop() calls. Since the [`std::mutex`](https://en.cppreference.com/w/cpp/thread/mutex.html) is neither copyable nor movable, so is the Stoppable::Task.
