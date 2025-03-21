// Copyright 2025 Artem Tyshkevich
#include "TimedDoor.h"
#include <stdexcept>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        throw std::runtime_error("Дверь открыта слишком долго!");
    }
}

TimedDoor::TimedDoor(int timeout) :
    iTimeout(timeout),
    isOpened(false) {
    adapter = new DoorTimerAdapter(*this);
}

TimedDoor::~TimedDoor() {
    delete adapter;
}

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
        throw std::runtime_error("Дверь открыта!");
    }
}

void Timer::sleep(int time) {}

void Timer::tregister(int time, TimerClient* c) {
    client = c;
    sleep(time);
    client->Timeout();
}
