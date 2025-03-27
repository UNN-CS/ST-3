// Copyright 2025 Konkov Ivan

#include "TimedDoor.h"
#include <stdexcept>
#include <thread>
#include <chrono>

// Реализация TimedDoor
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
    isOpened = true;
    adapter->Timeout();
}

void TimedDoor::lock() {
    isOpened = false;
}

int TimedDoor::getTimeOut() {
    return iTimeout;
}

void TimedDoor::throwState() {
    if (isOpened) {
        throw std::runtime_error("Дверь осталась открытой!");
    }
}

// Реализация DoorTimerAdapter
DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
    Timer timer;
    timer.tregister(door.getTimeOut(), this);
}

void Timer::tregister(int timeout, TimerClient* client) {
    this->client = client;
    std::thread([this, timeout, client]() {
        std::this_thread::sleep_for(std::chrono::seconds(timeout));
        client->Timeout();
    }).detach();
}

void Timer::sleep(int timeout) {
    std::this_thread::sleep_for(std::chrono::seconds(timeout));
    client->Timeout();
}
