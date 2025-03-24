// Copyright 2025 Artem Belov

#include "TimedDoor.h"
#include <thread>
#include <chrono>
#include <stdexcept>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
  if (door.isDoorOpened()) {
    throw std::runtime_error("Door has been opened too long!");
  }
}

TimedDoor::TimedDoor(int timeoutValue)
  : iTimeout(timeoutValue),
  isOpened(false),
  adapter(new DoorTimerAdapter(*this)) {
}

bool TimedDoor::isDoorOpened() {
  return isOpened;
}

void TimedDoor::unlock() {
  isOpened = true;
  Timer timer;
  timer.tregister(iTimeout, adapter);
}

void TimedDoor::lock() {
  isOpened = false;
}

int TimedDoor::getTimeOut() const {
  return iTimeout;
}

void TimedDoor::throwState() {
  if (isOpened) {
    throw std::runtime_error("Door is currently opened!");
  }
  else {
    throw std::runtime_error("Door is currently closed!");
  }
}

void Timer::tregister(int timeoutValue, TimerClient* c) {
  if (timeoutValue <= 0) {
    throw std::invalid_argument("Timeout value must be positive!");
  }
  this->client = c;
  sleep(timeoutValue);
  this->client->Timeout();
}

void Timer::sleep(int timeoutValue) {
  if (timeoutValue <= 0) {
    throw std::invalid_argument("Timeout value must be positive!");
  }
  std::this_thread::sleep_for(std::chrono::seconds(timeoutValue));
}