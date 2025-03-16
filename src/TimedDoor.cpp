// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <chrono>
#include <stdexcept>
#include <thread>

DoorTimerAdapter::DoorTimerAdapter(Door &door) : door(door) {}

void DoorTimerAdapter::Timeout() {
if (door.isDoorOpened()) {
    throw std::runtime_error("Door left open!");
}
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
  adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() { return isOpened; }

void TimedDoor::unlock() { isOpened = true; }

void TimedDoor::lock() { isOpened = false; }

int TimedDoor::getTimeOut() const { return iTimeout; }

void TimedDoor::throwState() { throw std::runtime_error("Door left open!"); }

DoorTimerAdapter *TimedDoor::getAdapter() { return adapter; }

void Timer::sleep(int milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void Timer::tregister(int timeout, TimerClient *client) {
  sleep(timeout);
  client->Timeout();
}
