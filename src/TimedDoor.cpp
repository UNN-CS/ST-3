// Copyright 2025 UNN-CS
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

void TimedDoor::lock() {
    isOpened = false;
}

void TimedDoor::unlock() {
    isOpened = true;
    Timer t;
    t.tregister(iTimeout, adapter);
}

int TimedDoor::getTimeOut() {
    return iTimeout;
}

void TimedDoor::throwState() {
    throw std::runtime_error("Door has been open too long!");
}

void Timer::sleep(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void Timer::tregister(int timeout, TimerClient* c) {
    client = c;
    sleep(timeout);
    client->Timeout();
}
