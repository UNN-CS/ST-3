// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <stdexcept>
#include <thread>
#include <chrono>

// –еализаци€ адаптера Ч только методы, без повторного class-объ€влени€
DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door)
    : door(door) {}

void DoorTimerAdapter::Timeout() {
    door.throwState();
}

// –еализаци€ TimedDoor

TimedDoor::TimedDoor(int timeout)
    : adapter(new DoorTimerAdapter(*this))
    , iTimeout(timeout)
    , isOpened(false)
{
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    isOpened = true;
    Timer t;
    t.tregister(iTimeout, adapter);
}

void TimedDoor::lock() {
    isOpened = false;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    if (isOpened) {
        throw std::runtime_error("Door left open");
    }
}

// –еализаци€ Timer

void Timer::sleep(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void Timer::tregister(int seconds, TimerClient* client) {
    this->client = client;
    sleep(seconds);
    try {
        client->Timeout();
    }
    catch (...) {
    }
}