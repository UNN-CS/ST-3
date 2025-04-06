// Copyright 2021 GHA Test Team
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

TimedDoor::TimedDoor(int timeout) {
    iTimeout = timeout;
    adapter = new DoorTimerAdapter(*this);
    isOpened = false;
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
    throw std::runtime_error("Door is opened!!!");
}

void Timer::sleep(int timeout) {
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
}

void Timer::tregister(int timeout, TimerClient* tc) {
    client = tc;
    sleep(timeout);
    client->Timeout();
}
