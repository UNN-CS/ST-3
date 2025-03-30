// Copyright 2025 Zinoviev Alexander
#include "TimedDoor.h"
#include <stdexcept>
#include <thread>
#include <chrono>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        door.throwState();
    }
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
    adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    isOpened = true;
    if (iTimeout > 0) {
        Timer timer;
        timer.tregister(iTimeout, adapter);
    }
}

void TimedDoor::lock() {
    isOpened = false;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    throw std::runtime_error("Door is still open after timeout");
}

void Timer::tregister(int timeout, TimerClient* client) {
    if (timeout > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
    }
    client->Timeout();
}
