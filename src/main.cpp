// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <exception>
#include <iostream>

int main() {
  // Таймаут в миллисекундах.
  // Если дверь остается открытой, будет выброшено исключение.
  TimedDoor tDoor(5);
  tDoor.lock();
  try {
    tDoor.unlock();
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}
