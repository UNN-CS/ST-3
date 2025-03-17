// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <thread>
#include <stdexcept>

// Реализация DoorTimerAdapter
DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        door.throwState();
    }
}

// Реализация TimedDoor
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

int TimedDoor::getTimeOut() {
    return iTimeout;
}

void TimedDoor::throwState() {
    throw std::runtime_error("Door is still opened after timeout!");
}

// Реализация Timer
void Timer::sleep(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void Timer::tregister(int milliseconds, TimerClient* client) {
    this->client = client;
    sleep(milliseconds);
    client->Timeout();
}
