// Copyright 2025 shvedovav

#include "TimedDoor.h"
#include <thread>
#include <chrono>
#include <stdexcept>
#include <memory>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        throw std::runtime_error("Error: The door has remained open for too long.");
    }
}

TimedDoor::TimedDoor(int to) :
    adapter(std::make_unique<DoorTimerAdapter>(*this)),
    iTimeout(std::max(to, 5)),
    isOpened(false) {}

bool TimedDoor::isDoorOpened() const {
    return isOpened;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::lock() {
    if (!isOpened) {
        throw std::logic_error("Error: Unable to lock. The door is already closed.");
    }
    isOpened = false;
}

void TimedDoor::unlock() {
    if (isOpened) {
        throw std::logic_error("Error: Unable to unlock. The door is already open.");
    }
    isOpened = true;
}

void TimedDoor::throwState() const {
    throw std::runtime_error(isOpened ? "Door status: Opened." : "Door status: Closed.");
}

void Timer::sleep(int timeoutSeconds) {
    if (timeoutSeconds <= 0) {
        throw std::invalid_argument("Error: Timeout duration must be greater than zero.");
    }
    std::this_thread::sleep_for(std::chrono::seconds(timeoutSeconds));
}

void Timer::tregister(int timeoutSeconds, TimerClient* client) {
    if (!client) {
        throw std::invalid_argument("Error: Timer client reference is null.");
    }
    if (timeoutSeconds <= 0) {
        throw std::invalid_argument("Error: Timeout duration must be positive.");
    }
    this->client = client;
    
    try {
        sleep(timeoutSeconds);
        client->Timeout();
    } catch (...) {
        throw std::runtime_error("Error: An issue occurred while running the timer.");
    }
}
