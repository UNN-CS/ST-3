// Copyright 2024 SecureDoor Team

#include "TimedDoor.h"
#include <chrono>
#include <thread>
#include <iostream>

DoorTimerAdapter::DoorTimerAdapter(const TimedDoor& door) : doorRef(door) {}

void DoorTimerAdapter::Timeout() {
    if (doorRef.isDoorOpened()) {
        doorRef.throwState();
    }
}

TimedDoor::TimedDoor(int timeoutSec) : isOpened(false), timeout(timeoutSec) {
    if (timeoutSec <= 0) {
        throw std::invalid_argument("Timeout must be positive");
    }
    adapter = new DoorTimerAdapter(*this);
}

TimedDoor::~TimedDoor() {
    delete adapter;
}

void TimedDoor::unlock() {
    if (!isOpened) {
        isOpened = true;
        Timer timer;
        timer.tregister(timeout, adapter);
    }
}

void TimedDoor::lock() {
    isOpened = false;
}

bool TimedDoor::isDoorOpened() const {
    return isOpened;
}

void TimedDoor::throwState() const {
    if (isOpened) {
        throw std::runtime_error("Door has been open for too long!");
    }
}

void Timer::tregister(int duration, TimerClient* client) {
    std::thread([duration, client]() {
        if (duration > 0) {
            std::this_thread::sleep_for(std::chrono::seconds(duration));
        }
        if (client) {
            try {
                client->Timeout();
            } catch (const std::exception& e) {
                std::cerr << "Timer error: " << e.what() << '\n';
            }
        }
    }).detach();
}
