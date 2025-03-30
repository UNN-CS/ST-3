// Copyright 2025

#include "TimedDoor.h"
#include <thread> // NOLINT [build/c++11]
#include <chrono> // NOLINT [build/c++11]
#include <stdexcept>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        throw std::runtime_error("This door has been opened so long...");
    }
}

TimedDoor::TimedDoor(int to) : adapter(new DoorTimerAdapter(*this)),
iTimeout(to), isOpened(false) {}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::lock() {
    if (!isOpened) {
        throw std::logic_error("This door is closed now!");
    }
    isOpened = false;
}

void TimedDoor::unlock() {
    if (isOpened) {
        throw std::logic_error("This door is opened now!");
    }
    isOpened = true;
}

void TimedDoor::throwState() {
    if (isOpened) {
        throw std::runtime_error("This door is opened");
    } else {
        throw std::runtime_error("This door is closed");
    }
}

void Timer::sleep(int to) {
    std::this_thread::sleep_for(std::chrono::seconds(to));
}

void Timer::tregister(int to, TimerClient* client) {
    if (client == nullptr) {
        throw std::invalid_argument("Client cannot be null");
    }
    this->client = client;
    sleep(to);
    client->Timeout();
}