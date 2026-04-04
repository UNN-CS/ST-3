// Copyright 2021 GHA Test Team
#include "TimedDoor.h"

#include <stdexcept>

// DoorTimerAdapter

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        door.throwState();
    }
}

// TimedDoor

TimedDoor::TimedDoor(int timeout)
    : iTimeout(timeout), isOpened(false) {
    adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    isOpened = true;
}

void TimedDoor::lock() {
    isOpened = false;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    throw std::runtime_error("Door is open too long!");
}

// Timer
void Timer::sleep(int) {
}

void Timer::tregister(int timeout, TimerClient* client) {
    this->client = client;
}