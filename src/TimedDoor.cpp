// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <iostream>
#include <stdexcept>
#include <thread> // NOLINT
#include <chrono> // NOLINT

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
  door.throwState();
}

TimedDoor::TimedDoor(int timeout)
    : adapter(new DoorTimerAdapter(*this)), iTimeout(timeout), isOpened(false) {}

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
    throw std::runtime_error("Door is opened too long!");
  }
}

void Timer::tregister(int timeout, TimerClient* client) {
  if (client) {
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
    client->Timeout();
  }
}