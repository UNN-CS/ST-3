// Copyright 2021 GHA Test Team
#include <thread>
#include <chrono>
#include <stdexcept>
#include "TimedDoor.h"

TimedDoor::TimedDoor(int timeout)
    : iTimeout(timeout), isOpened(false), adapter(nullptr) {}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    isOpened = true;
}

void TimedDoor::lock() {
    isOpened = false;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    if (isOpened) {
        throw std::runtime_error("Error");;
    }
}

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {
    d.setAdapter(this);
}

void DoorTimerAdapter::Timeout() {
    std::this_thread::sleep_for(std::chrono::seconds(door.getTimeOut()));
    if (door.isDoorOpened()) {
        door.throwState();
    }
}

void Timer::sleep(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void Timer::tregister(int timeout, TimerClient * c) {
    client = c;

        std::thread([=]() {
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            if (client) {
                client->Timeout();
            }
        }).detach();
}
