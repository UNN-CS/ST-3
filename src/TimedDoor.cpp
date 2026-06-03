// Copyright 2021 GHA Test Team
#include "TimedDoor.h"

#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <thread>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
  Timer timer;
  timer.tregister(door.getTimeOut(), this);
  door.throwState();
}

TimedDoor::TimedDoor(int timeout)
    : adapter(new DoorTimerAdapter(*this)),
      iTimeout(timeout),
      isOpened(false) {}

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

int TimedDoor::getTimeOut() const {
  return iTimeout;
}

void TimedDoor::throwState() {
  if (isOpened) {
    throw std::runtime_error("Door is still open");
  }
}

void Timer::sleep(int timeout) {
  const int delay = std::max(0, std::min(timeout, 1));
  std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

void Timer::tregister(int timeout, TimerClient* client) {
  this->client = client;
  sleep(timeout);
}
