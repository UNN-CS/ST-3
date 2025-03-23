// Copyright 2025 Kalinin Dmitry

#include <stdexcept>
#include <thread> // NOLINT [build/c++11]
#include <chrono> // NOLINT [build/c++11]

#include "TimedDoor.h"

DoorTimerAdapter::DoorTimerAdapter(TimedDoor & door_) : door(door_) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened())
        throw std::runtime_error("Time's up!");
    return;
}

TimedDoor::TimedDoor(int timeout_) : iTimeout(timeout_), isOpened(false) {
    adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    if (isOpened)
        throw std::logic_error("Door is open!!!");
    isOpened = true;
}

void TimedDoor::lock() {
    if (!isOpened)
        throw std::logic_error("Door is close!!!");
    isOpened = false;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    adapter->Timeout();
}

void Timer::sleep(int time_) {
    std::this_thread::sleep_for(std::chrono::seconds(time_));
}

void Timer::tregister(int time_, TimerClient* client_) {
    this->client = client_;
    sleep(time_);
    client_->Timeout();
}
