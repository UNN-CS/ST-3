// Copyright 2025 Konkov Ivan

#include "TimedDoor.h"
#include <stdexcept>
#include <thread>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        door.throwState();
    }
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
    adapter = new DoorTimerAdapter(*this);
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
    throw std::runtime_error("Door opened too long!");
}

void Timer::tregister(int timeout, TimerClient* client) {
    std::thread([client, timeout]() {
        std::this_thread::sleep_for(std::chrono::seconds(timeout));
        client->Timeout();
    }).detach();
}
