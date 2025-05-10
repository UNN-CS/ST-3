// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <chrono>
#include <stdexcept>
#include <thread>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
    door.throwState();
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
    adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() { return isOpened; }

void TimedDoor::unlock() {
    isOpened = true;
    Timer timer;
    timer.tregister(iTimeout, adapter);
}

TimedDoor::~TimedDoor() { delete adapter; }

void TimedDoor::lock() { isOpened = false; }

int TimedDoor::getTimeOut() const { return iTimeout; }

void Timer::tregister(int timeout, TimerClient* client) {
    std::thread([client, timeout]() {
        std::this_thread::sleep_for(std::chrono::seconds(timeout));
        client->Timeout();
    }).detach();
}

void TimedDoor::throwState() {
    if (isOpened) {
        throw std::runtime_error("The door had been open too long");
    }
}

void Timer::sleep(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
