// Copyright 2025 Mikhail Burykin

#include "TimedDoor.h"
#include <thread>
#include <chrono>
#include <stdexcept>

void Timer::sleep(const int time) {
    std::this_thread::sleep_for(std::chrono::seconds(time));
}

void Timer::tregister(int time, TimerClient* client) {
    this->client = client;
    std::thread([time, client]() {
        std::this_thread::sleep_for(std::chrono::seconds(time));
        try {
            client->Timeout();
        } catch (const std::runtime_error&) {
            // ignore
        }
    }).detach();
}

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        throw std::runtime_error("Door is still open!");
    }
}

TimedDoor::TimedDoor(const int timeout)
    : adapter(new DoorTimerAdapter(*this)),
        iTimeout(timeout), isOpened(false) {}

TimedDoor::~TimedDoor() {
    delete adapter;
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    isOpened = true;
    Timer timer{};
    timer.tregister(iTimeout, adapter);
}

void TimedDoor::lock() {
    isOpened = false;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    // isn't used
}
