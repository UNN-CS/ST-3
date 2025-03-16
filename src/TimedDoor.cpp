// Copyright 2021 GHA Test Team
#include "../include/TimedDoor.h"
#include <thread>
#include <chrono>
#include <stdexcept>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
  door.throwState();
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false),
     adapter(new DoorTimerAdapter(*this)) {}

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
    throw std::runtime_error("Door is still open after timeout!");
  }
}

void Timer::tregister(int timeout, TimerClient* client) {
  std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
  client->Timeout();
}

TimedDoor::~TimedDoor() {
  delete adapter;
}
