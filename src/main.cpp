// Copyright 2026 GHA Test Team

#include "TimedDoor.h"
#include <iostream>
#include <thread>

int main() {
  try {
    std::cout << "=== Smart Door with Timer Demo ===" << std::endl;

    // Create door with 3 second timeout
    TimedDoor smartDoor(3);
    std::cout << "Door created. Timeout set to: "
              << smartDoor.getTimeOut() << " seconds" << std::endl;

    // Close the door
    smartDoor.lock();
    std::cout << "Door is locked (closed)" << std::endl;

    // Open the door
    std::cout << "\nOpening the door..." << std::endl;
    smartDoor.unlock();
    std::cout << "Door is opened. Timer started!" << std::endl;

    // Simulate working with door
    std::cout << "Waiting for 4 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(4));

    // Check state
    if (smartDoor.isDoorOpened()) {
      std::cout << "Door is still open!" << std::endl;
      smartDoor.throwState();  // This will throw exception
    }
  } catch (const std::runtime_error& e) {
    std::cout << "\n[ALERT] " << e.what() << std::endl;
    std::cout << "The door was left open for too long!" << std::endl;
  }

  std::cout << "\n=== Demo with closing before timeout ===" << std::endl;

  try {
    TimedDoor anotherDoor(3);
    anotherDoor.unlock();
    std::cout << "Door opened, closing after 1 second..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    anotherDoor.lock();
    std::cout << "Door closed before timeout. No alarm!" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    anotherDoor.throwState();  // Should not throw
    std::cout << "Everything is fine!" << std::endl;
  } catch (const std::runtime_error& e) {
    std::cout << "Exception: " << e.what() << std::endl;
  }

  return 0;
}
