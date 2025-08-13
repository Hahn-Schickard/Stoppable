#include "Stoppable/Task.hpp"

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

using namespace std;

int main() {
  {
    auto task = Stoppable::Task([]() { this_thread::sleep_for(1ms); },
        [](const std::exception_ptr&) {});
    task.start();
    this_thread::sleep_for(10ms);
    task.stop();
  }

  cout << "Integration test successful" << endl;

  exit(EXIT_SUCCESS);
}
