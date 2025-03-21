// Copyright 2025 Artem Tyshkevich
#include "TimedDoor.h"
#include <iostream>

int main() {
  TimedDoor tDoor(5);
  tDoor.lock();
  tDoor.unlock();

  return 0;
}
