// Copyright 2021 GHA Test Team

#include <cstdint>
#include <iostream>
#include <stdexcept>

#include "TimedDoor.h"

int main() {
  TimedDoor door(5);
  std::cout << "Timeout: " << door.getTimeOut() << " ms\n";

  door.lock();
  std::cout << "Door opened? " << door.isDoorOpened() << "\n";

  door.unlock();
  std::cout << "Door opened? " << door.isDoorOpened() << "\n";

  Timer timer;
  DoorTimerAdapter adapter(door);
  try {
    timer.tregister(door.getTimeOut(), &adapter);
  } catch (const std::runtime_error& e) {
    std::cout << "Alarm: " << e.what() << "\n";
  }
  return 0;
}
