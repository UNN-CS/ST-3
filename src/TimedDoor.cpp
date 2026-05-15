// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <thread>
#include <chrono>
#include <stdexcept>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
  Timer timer;

  timer.tregister(door.getTimeOut(), this);

  if (door.isDoorOpened()) {
    door.throwState();
  }
}


TimedDoor::TimedDoor(int timeout) {
  iTimeout = timeout;
  isOpened = false;
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

int TimedDoor::getTimeOut() const {
  return iTimeout;
}

void TimedDoor::throwState() {
  throw std::runtime_error("Door is still opened!");
}

void Timer::tregister(int seconds, TimerClient* cl) {
  client = cl;

  std::this_thread::sleep_for(std::chrono::seconds(seconds));
}
