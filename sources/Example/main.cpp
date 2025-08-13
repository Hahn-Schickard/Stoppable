#include "Task.hpp"

#include <functional>
#include <iostream>
#include <thread>

using namespace std;
using namespace Stoppable;

int main() {
  {
    size_t counter = 0;
    auto task = make_unique<Task>(
        [&counter]() {
          cout << "Running cycle " << counter << endl;
          counter++;
          this_thread::sleep_for(10ms);
        },
        [](const std::exception_ptr& ex_ptr) {
          try {
            if (ex_ptr) {
              rethrow_exception(ex_ptr);
            }
          } catch (const exception& e) {
            cerr << e.what() << endl;
          }
        });
    task->start();
    this_thread::sleep_for(100ms);
    task->stop();
  }

  cout << "All Tasks have bean cleaned up!" << endl;

  exit(EXIT_SUCCESS);
}
