// Copyright 2025

#include "TimedDoor.h"
#include <chrono>
#include <stdexcept>
#include <thread>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        door.isThrow = true;
    }
}

TimedDoor::TimedDoor(int timeoutValue) :
    iTimeout(timeoutValue),
    isOpened(false),
    adapter(new DoorTimerAdapter(*this)) {
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    isOpened = true;

    th = new std::thread([&]() {
        Timer timer;
    timer.tregister(iTimeout, adapter);
        });
}

void TimedDoor::lock() {
    throwState();
    isOpened = false;

    if (th) {
        th->join();
        delete th;
    }
    th = nullptr;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    if (isThrow) {
        throw std::runtime_error("Door has been opened too long!");
    }
}

void Timer::tregister(int timeoutValue, TimerClient* c) {
    if (timeoutValue <= 0) {
        throw std::invalid_argument("Timeout value must be positive!");
    }

    this->client = c;
    sleep(timeoutValue);
    this->client->Timeout();
}

void Timer::sleep(int timeoutValue) {
    if (timeoutValue <= 0) {
        throw std::invalid_argument("Timeout value must be positive!");
    }

    std::this_thread::sleep_for(std::chrono::seconds(timeoutValue));
}
