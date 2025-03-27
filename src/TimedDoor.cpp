// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <thread>
#include <chrono>
#include <stdexcept>
#include <iostream>

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        door.throwState();
    }
}

TimedDoor::TimedDoor(int iTimeout) {
    this->iTimeout = iTimeout;
    this->adapter = new DoorTimerAdapter(*this);
    this->isOpened = false;
}

TimedDoor::~TimedDoor() {
    if (th.joinable()) {
        th.join();
    }
    delete this->adapter;
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}
  
void TimedDoor::unlock() {
    isOpened = true;
    th = std::thread([&](){
        Timer timer;
        timer.tregister(iTimeout, adapter);
    });
}
  
void TimedDoor::lock() {
    isOpened = false;
    if (th.joinable()) {
        th.join();
    }
}

int  TimedDoor::getTimeOut() const{
    return iTimeout;
}

void TimedDoor::throwState() {
    std::cout << "The door is open\n";
}

void Timer::sleep(int timeout) {
    std::this_thread::sleep_for(std::chrono::seconds(timeout));
}

void Timer::tregister(int timeout, TimerClient* client) {
    this->client = client;
    sleep(timeout);
    client->Timeout();
}