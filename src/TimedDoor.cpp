// Copyright 2025 Kavtorev Dmitry

#include <stdexcept>
#include <thread> // NOLINT [build/c++11]
#include <chrono> // NOLINT [build/c++11]

#include "TimedDoor.h"

void Timer::sleep(int seconds) {
     std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void Timer::tregister(int seconds, TimerClient* timerClient) {
    client = timerClient;
    sleep(seconds);
    client->Timeout();
    client = nullptr;
}

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        throw std::runtime_error("Door has been opened for too long!");
    }
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
    adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    if (isOpened) {
        throw std::logic_error("Door already unlocked");
    }
    isOpened = true;
}

void TimedDoor::lock() {
    if (!isOpened) {
        throw std::logic_error("Door already locked");
    }
    isOpened = false;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    if (isOpened) {
        throw std::runtime_error("Door is opened!");
    } else {
        throw std::runtime_error("Door is closed!");
    }
}
