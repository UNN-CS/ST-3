// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <iostream>
using namespace std;

int main() {
  int time = 5;
  TimedDoor tDoor(time);
  tDoor.unlock();
  std::this_thread::sleep_for(std::chrono::seconds(time + 2));
  tDoor.lock();

  return 0;
}
