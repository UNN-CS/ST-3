// Copyright 2021 GHA Test Team

#include "TimedDoor.h"
#include <thread>
#include <chrono>
#include <stdexcept>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        door.throwState();
    }
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
    adapter = new DoorTimerAdapter(*this);
}

TimedDoor::~TimedDoor() {
    delete adapter;
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

int TimedDoor::getTimeOut() {
    return iTimeout;
}

void TimedDoor::throwState() {
    throw std::runtime_error("Door opened too long!");
}

void Timer::sleep(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void Timer::tregister(int timeout, TimerClient* client) {
    client = client;
    sleep(timeout);
    client->Timeout();
}
