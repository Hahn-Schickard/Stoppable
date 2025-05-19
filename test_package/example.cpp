#include "Stoppable/Task.hpp"

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

using namespace std;

int main() {
  {
    auto task = make_unique<Stoppable::Task>(
        [](const exception_ptr& ex_ptr) {
          try {
            if (ex_ptr) {
              rethrow_exception(ex_ptr);
            }
          } catch (const exception& e) {
            cout << e.what() << endl;
          }
        },
        []() { throw runtime_error("Test throwing"); });
    task->start();
    this_thread::sleep_for(10ms);
  }

  cout << "Integration test successful" << endl;

  exit(EXIT_SUCCESS);
}
