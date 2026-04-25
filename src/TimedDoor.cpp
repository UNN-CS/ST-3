// Copyright 2021 GHA Test Team
#include <stdexcept>

#include "TimedDoor.h"

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& timed_door) : door(timed_door) {
}

void DoorTimerAdapter::Timeout() {
  if (door.isDoorOpened()) {
    door.throwState();
  }
}

TimedDoor::TimedDoor(int timeout)
    : adapter(new DoorTimerAdapter(*this)),
      iTimeout(timeout),
      isOpened(false) {
}

bool TimedDoor::isDoorOpened() {
  return isOpened;
}

void TimedDoor::unlock() {
  isOpened = true;
  Timer timer;
  try {
    timer.tregister(iTimeout, adapter);
  } catch (...) {
  }
}

void TimedDoor::lock() {
  isOpened = false;
}

int TimedDoor::getTimeOut() const {
  return iTimeout;
}

void TimedDoor::throwState() {
  if (isOpened) {
    throw std::runtime_error("Door is opened");
  }
}

void Timer::sleep(int) {
}

void Timer::tregister(int timeout, TimerClient* timer_client) {
  client = timer_client;
  sleep(timeout);
  if (client != nullptr) {
    client->Timeout();
  }
}
