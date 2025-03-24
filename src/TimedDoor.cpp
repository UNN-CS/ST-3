// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <stdexcept>
#include <chrono>
#include <thread>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

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
    if (isOpened) {
        throw std::runtime_error("Door is open after timeout!");
    }
}

void Timer::sleep(int seconds) {
    if (seconds > 0) {
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
    }
}

void Timer::tregister(int time, TimerClient* c) {
    client = c;

    std::thread this_thread([time, c]() {
        if (time > 0) {
            std::this_thread::sleep_for(std::chrono::seconds(time));
        }
        if (c) {
            c->Timeout();
        }
        });
    timer.detach();
}
