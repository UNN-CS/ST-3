// Copyright 2021 GHA Test Team

#include "TimedDoor.h"
#include <chrono>
#include <stdexcept>
#include <thread>
#include <iostream>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor &timedDoor) : door(timedDoor) {
}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        door.throwState();
    }
}

TimedDoor::TimedDoor(int timeout) {
    if (timeout <= 0) {
        throw std::invalid_argument("TimedDoor: Timeout must be positive");
    }
    iTimeout = timeout;
    isOpened = false;
    adapter = new DoorTimerAdapter(*this);
}

void TimedDoor::unlock() {
    isOpened = true;
    auto timer = std::make_shared<Timer>();
    timer->tregister(iTimeout, adapter);
}

void TimedDoor::lock() {
    isOpened = false;
}

TimedDoor::~TimedDoor() {
    delete adapter;
}

void TimedDoor::throwState() {
    throw std::runtime_error("Timeout: Door remained open");
}

int TimedDoor::getTimeout() {
    return iTimeout;
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void Timer::tregister(int seconds, TimerClient *timerClient) {
    client = timerClient;
    std::thread(
        [seconds, timerClient]()-> void {
            try {
                sleep(seconds);
                timerClient->Timeout();
            } catch (const std::exception &e) {
                std::cerr << "Timer: Exception caught: " << e.what() << std::endl;
            }
        }
    ).detach();
}

void Timer::sleep(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}
