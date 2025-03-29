// Copyright 2025 Alexey Gromov
#include "TimedDoor.h"
#include <iostream>

int main() {
  TimedDoor tDoor(5);
  tDoor.lock();
  tDoor.unlock();

  return 0;
}
