#include "Stoppable/StoppableTask.hpp"

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

int main() {
  auto task = make_unique<StoppableTask>(make_shared<StoppableImplementation>(),
                                         "Runner");

  cout << "Starting " << task->getName() << endl;
  task->startTask();
  this_thread::sleep_for(0.5s);
  task->stopTask();
  exit(EXIT_SUCCESS);
}