// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <iostream>

class RealTimer : public Timer {
 public:
  void tregister(int, TimerClient*) override {}
};

int main() {
  RealTimer timer;
  TimedDoor tDoor(5, timer);
  tDoor.lock();
  tDoor.unlock();
  return 0;
}
