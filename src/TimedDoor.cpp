// Copyright 2021 GHA Test Team

#include "TimedDoor.h"
#include <stdexcept>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
    door.throwState();
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false), timer(new Timer()) {
    adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    isOpened = true;
    timer->tregister(iTimeout, adapter);
}

void TimedDoor::lock() {
    isOpened = false;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    if (isOpened) {
        throw std::runtime_error("Door is still open!");
    }
}

Timer* TimedDoor::getTimer() {
    return timer;
}

void Timer::tregister(int timeout, TimerClient* client) {
    this->client = client;
}

void Timer::trigger() {
    if (client) {
        client->Timeout();
    }
}
