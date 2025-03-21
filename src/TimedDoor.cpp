// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <stdexcept>
#include <memory>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        door.throwState();
    }
}

TimedDoor::TimedDoor(int timeout, Timer* timer) 
    : timer(timer), iTimeout(timeout), isOpened(false) {
    adapter = std::make_unique<DoorTimerAdapter>(*this);
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    isOpened = true;
    timer->tregister(iTimeout, adapter.get());
}

void TimedDoor::lock() {
    isOpened = false;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    throw std::runtime_error("Door is still open after timeout!");
}
