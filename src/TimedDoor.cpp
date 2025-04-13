// Copyright 2021 GHA Test Team
#include "TimedDoor.h"

#include <stdexcept>
#include <thread>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {}

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
    throw std::runtime_error("Door is still open");
  }
}

void Timer::tregister(int timeout, TimerClient* client) {
  this->client = client;
  sleep(timeout);
  client->Timeout();
}

void Timer::sleep(int timeout) {
  std::this_thread::sleep_for(std::chrono::seconds(timeout));
}

void DoorTimerAdapter::Timeout() {
  door.throwState();
}
