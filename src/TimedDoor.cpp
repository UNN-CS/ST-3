// Copyright 2025 Vasilev Sergey

#include "TimedDoor.h"
#include <stdexcept>
#include <thread>
#include <chrono>
#include <string>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& timedDoor) : door(timedDoor) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        throw std::runtime_error("Door alert: Left open for too long!");
    }
}

TimedDoor::TimedDoor(int timeout) :
    iTimeout(timeout > 0 ? timeout : 10),
    isOpened(false) {
    adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::lock() {
    if (isOpened) {
        isOpened = false;
    } else {
        throw std::logic_error("Door is already locked");
    }
}

void TimedDoor::unlock() {
    if (!isOpened) {
        isOpened = true;
    } else {
        throw std::logic_error("Door is already unlocked");
    }
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    if (isOpened) {
        throw std::runtime_error("Current door state: Open");
    } else {
        throw std::runtime_error("Current door state: Closed");
    }
}

void Timer::sleep(int seconds) {
    if (seconds <= 0) {
        throw std::invalid_argument("Invalid timer duration");
    }
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void Timer::tregister(int seconds, TimerClient* timerClient) {
    if (!timerClient) {
        throw std::invalid_argument("Invalid timer client");
    }
    client = timerClient;
    try {
        sleep(seconds);
        client->Timeout();
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Timer error: ") + e.what());
    }
}
