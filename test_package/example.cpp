#include "Stoppable/Task.hpp"

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

using namespace std;

int main() {
  auto cycle = []() { this_thread::sleep_for(1ms); };
  auto handler = [](const exception_ptr&) {};
  {
    auto token = Stoppable::makeStopToken();
    auto routine = Stoppable::Routine(token, cycle, handler);
  }
  {
    auto task = Stoppable::Task(cycle, handler);
  }

  cout << "Integration test successful" << endl;

  exit(EXIT_SUCCESS);
}
