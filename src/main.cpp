// Copyright 2021 GHA Test Team
#include "TimedDoor.h"

#include <iostream>
#include <stdexcept>

int main() {
  TimedDoor door(50);
  door.lock();
  std::cout << "Door locked, timeout_ms=" << door.getTimeOut() << '\n';
  try {
    door.unlock();
    std::cout << "Door closed before alarm (no exception)\n";
  } catch (const std::runtime_error& e) {
    std::cout << "Alarm: " << e.what() << '\n';
  }
  return 0;
}
