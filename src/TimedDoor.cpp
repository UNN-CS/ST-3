// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <stdexcept>
#include <thread>
#include <chrono>

// Реализация конструктора адаптера
DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

// Метод Timeout вызывается по истечении таймера
void DoorTimerAdapter::Timeout() {
    door.throwState();
}

// Конструктор TimedDoor
TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
    adapter = new DoorTimerAdapter(*this);
}

// Деструктор TimedDoor
TimedDoor::~TimedDoor() {
    delete adapter;
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    isOpened = true;
    Timer timer;
    timer.tregister(iTimeout, adapter);
}

void TimedDoor::lock() {
    isOpened = false;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    if (isOpened) {
        throw std::runtime_error("Door left open too long!");
    }
}

void Timer::tregister(int timeout, TimerClient* client) {
    client = client;
    std::this_thread::sleep_for(std::chrono::seconds(timeout));
    client->Timeout();
}
