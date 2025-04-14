// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <iostream>
#include <chrono>
#include <stdexcept>
#include <thread>

DoorTimerAdapter::DoorTimerAdapter(Door& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        try {
            if (auto* timedDoor = dynamic_cast<TimedDoor*>(&door)) {
                std::lock_guard<std::mutex> lock(timedDoor->mtx);
                timedDoor->isThrow = true;
            }
            door.lock();
        } catch (...) {
            std::cerr << "Error in Timeout()" << std::endl;
        }
    }
}

TimedDoor::TimedDoor(int timeoutValue)
    : iTimeout(timeoutValue), isOpened(false), isThrow(false),
      adapter(std::make_unique<DoorTimerAdapter>(*this)) {}

TimedDoor::~TimedDoor() {
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

bool TimedDoor::isDoorOpened() {
    std::lock_guard<std::mutex> lock(mtx);
    return isOpened;
}

void TimedDoor::unlock() {
    std::lock_guard<std::mutex> lock(mtx);
    isOpened = true;
    isThrow = false;

    threads.push_back(std::thread([this]() {
        Timer timer;
        timer.tregister(iTimeout, adapter.get());
    }));
}

void TimedDoor::lock() {
    try {
        throwState();
        std::lock_guard<std::mutex> lock(mtx);
        isOpened = false;
    } catch (const std::runtime_error& e) {
        std::cerr << "Lock failed: " << e.what() << std::endl;
        throw;
    }
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    std::lock_guard<std::mutex> lock(mtx);
    if (isThrow) {
        isThrow = false;
        throw std::runtime_error("Door has been opened too long!");
    }
}

void Timer::tregister(int timeoutValue, TimerClient* c) {
    if (timeoutValue <= 0) {
        throw std::invalid_argument("Timeout value must be positive!");
    }
    this->client = c;
    sleep(timeoutValue);
    this->client->Timeout();
}

void Timer::sleep(int timeoutValue) {
    if (timeoutValue <= 0) {
        throw std::invalid_argument("Timeout value must be positive!");
    }
    std::this_thread::sleep_for(std::chrono::seconds(timeoutValue));
}
