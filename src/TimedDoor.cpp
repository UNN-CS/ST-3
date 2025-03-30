// Copyright 2025 Yaroslav

#include "TimedDoor.h"
#include <thread> // NOLINT [build/c++11]
#include <chrono> // NOLINT [build/c++11]
#include <stdexcept>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        throw std::runtime_error("Door is still open after timeout!");
    }
    return;
}

TimedDoor::TimedDoor(int timeout) : adapter(new DoorTimerAdapter(*this)),
    iTimeout(timeout), isOpened(false) {}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    if (isOpened) {
        throw std::logic_error("Door is already unlocked");
    }
    isOpened = true;
}

void TimedDoor::lock() {
    if (!isOpened)
        throw std::logic_error("Door is already closed");
    isOpened = false;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    if (isOpened) {
        throw std::runtime_error("Door is open!");
    }
}

void Timer::tregister(int timeout, TimerClient* client) {
    this->client = client;
    sleep(timeout);
    client->Timeout();
}

void Timer::sleep(int timeout) {
    std::this_thread::sleep_for(std::chrono::seconds(timeout));
}
