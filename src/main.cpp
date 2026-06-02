// Copyright 2021 GHA Test Team

#include "TimedDoor.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    try {
        TimedDoor door(3000);
        Timer timer;

        std::cout << "Door created. Timeout: " << door.getTimeOut()
                  << " ms" << std::endl;

        door.unlock();
        std::cout << "Door opened" << std::endl;

        DoorTimerAdapter adapter(door);
        timer.tregister(door.getTimeOut(), &adapter);
    } catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
    }

    return 0;
}
