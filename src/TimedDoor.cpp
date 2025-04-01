// Copyright 2025 Korneeva Ekaterina
#include "TimedDoor.h"
#include <iostream> 
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

TimedDoor::~TimedDoor() {
    delete adapter;
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    if (!isOpened) {
        isOpened = true;
        if (adapter) {
            Timer timer;
            timer.tregister(iTimeout, adapter);
        }
    }
}

void TimedDoor::lock() {
    isOpened = false;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    throw std::runtime_error("Door opened too long");
}

void Timer::sleep(int ms) {
    if (ms <= 0) {
        throw std::invalid_argument("Timer duration must be positive.");
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void Timer::tregister(int timeout, TimerClient* client) {
    if (!client || timeout <= 0) {
        throw std::invalid_argument("Invalid timer parameters");
    }

    std::thread([client, timeout]() {
        try {
            std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
            if (client) {
                client->Timeout();
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Timer thread error: " << e.what() << std::endl;
        }
        }).detach();
}
