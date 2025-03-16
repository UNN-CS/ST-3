// Copyright 2025 Vladlen Korablev

#include "TimedDoor.h"
#include <thread>
#include <chrono>
#include <string>
#include <stdexcept>
#include <memory>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        throw std::runtime_error("Door has been open too long!");
    }
}

TimedDoor::TimedDoor(int to) :
    adapter(new DoorTimerAdapter(*this)),
    iTimeout(to > 0 ? to : 5),
    isOpened(false) {}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::lock() {
    if (!isOpened) {
        throw std::logic_error("Cannot lock: Door is already closed");
    }
    isOpened = false;
}

void TimedDoor::unlock() {
    if (isOpened) {
        throw std::logic_error("Cannot unlock: Door is already open");
    }
    isOpened = true;
}

void TimedDoor::throwState() {
    if (isOpened) {
        throw std::runtime_error("Door status: Open");
    } else {
        throw std::runtime_error("Door status: Closed");
    }
}

void Timer::sleep(int timeoutSeconds) {
    if (timeoutSeconds <= 0) {
        throw std::invalid_argument("Timeout must be positive");
    }
    std::this_thread::sleep_for(std::chrono::seconds(timeoutSeconds));
}

void Timer::tregister(int timeoutSeconds, TimerClient* client) {
    if (client == nullptr) {
        throw std::invalid_argument("Client cannot be null");
    }
    if (timeoutSeconds <= 0) {
        throw std::invalid_argument("Timeout must be positive");
    }
    this->client = client;
    try {
        sleep(timeoutSeconds);
        client->Timeout();
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Timer error: ") + e.what());
    }
}
