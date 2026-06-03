// Copyright 2024 Nazyrov A.A.

#include <iostream>
#include "TimedDoor.h"

int main() {
  try {
    TimedDoor door(5);
    std::cout << "Door created with 5 second timeout\n";

    door.Unlock();
    std::cout << "Door unlocked. Timer started...\n";

    // Simulate closing door before timeout
    door.Lock();
    std::cout << "Door locked before timeout - no exception\n";
  } catch (const std::exception& e) {
    std::cout << "Exception: " << e.what() << "\n";
  }

  return 0;
}
