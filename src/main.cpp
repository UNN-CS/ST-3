// Copyright 2021 GHA Test Team

#include "TimedDoor.h"

#include <iostream>

int main() {
  try {
    TimedDoor tDoor(5);
    tDoor.lock();
    tDoor.unlock();
  } catch (const std::exception& e) {
    std::cout << e.what() << '\n';
  }
  return 0;
}
