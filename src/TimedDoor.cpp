// src/TimedDoor.cpp
// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <stdexcept>
#include <thread>
#include <chrono>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& doorRef)
    : door(doorRef) {
}

void DoorTimerAdapter::Timeout() {
    door.throwState();
}

TimedDoor::TimedDoor(int timeout)
    : iTimeout(timeout), isOpened(false) {
    adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    if (isOpened) {
        throw std::logic_error("Door is already unlocked.");
    }
    isOpened = true;
}

void TimedDoor::lock() {
    if (!isOpened) {
        throw std::logic_error("Door is already locked.");
    }
    isOpened = false;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    throw std::runtime_error("Door state error.");
}

void Timer::tregister(int timeout, TimerClient* clientArg) {
    if (clientArg == nullptr) {
        throw std::invalid_argument("TimerClient pointer cannot be null.");
    }
    if (timeout <= 0) {
        throw std::invalid_argument("Timeout must be positive.");
    }
    client = clientArg;
    std::this_thread::sleep_for(std::chrono::seconds(timeout));
    client->Timeout();
}

}
