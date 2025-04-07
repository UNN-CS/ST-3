// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <stdexcept>
#include <chrono>
#include <thread>

TimerClient *client = nullptr;
void Timer::tregister(int timeout, TimerClient *c) {
    client = c;
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
    if (client)
        client->Timeout();
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
    adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    isOpened = true;
    Timer timer;
    timer.tregister(iTimeout, adapter);
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

DoorTimerAdapter::DoorTimerAdapter(TimedDoor &d) : door(d) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        door.throwState();
    }
}
