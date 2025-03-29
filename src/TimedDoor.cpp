// Copyright 2025 Alexey Gromov
#include "TimedDoor.h"
#include <stdexcept>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
    door.getTimeout();
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
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

void TimedDoor::getTimeout() {
    if (isDoorOpened()) {
        throwState();
    }
}

void TimedDoor::throwState() {
    throw std::runtime_error("Door is still open after timeout");
}

void Timer::sleep(int seconds) {}

void Timer::registerTimer(int seconds, TimerClient* client) {
    this->client = client;
    sleep(seconds);
    client->Timeout();
}
