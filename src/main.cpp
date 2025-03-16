// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <iostream>

int main() {
  try {
    TimedDoor tDoor(5000);
    Timer timer;

    tDoor.unlock();
    timer.tregister(tDoor.getTimeOut(),
                    tDoor.getAdapter());
  } catch (const std::runtime_error &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}
