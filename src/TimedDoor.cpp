// Copyright 2021 GHA Test Team

#include "TimedDoor.h"

#include <chrono>
#include <stdexcept>
#include <thread>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& timed_door) : door(timed_door) {}

void DoorTimerAdapter::Timeout() {
  door.throwState();
}

TimedDoor::TimedDoor(int timeout)
    : adapter(nullptr), iTimeout(timeout), isOpened(false) {
  adapter = new DoorTimerAdapter(*this);
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
    throw std::runtime_error("Door is opened too long");
  }
}

void Timer::sleep(int timeout) {
  std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
}

void Timer::tregister(int timeout, TimerClient* timeout_client) {
  client = timeout_client;
  sleep(timeout);
  client->Timeout();
}
