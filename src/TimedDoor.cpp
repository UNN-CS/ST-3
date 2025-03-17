// Copyright 2021 GHA Test Team

#include "TimedDoor.h"
#include <thread>
#include <stdexcept>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
  if (door.isDoorOpened()) {
    door.throwState();
  }
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
  adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() const {
  return isOpened;
}

void TimedDoor::unlock() {
  isOpened = true;
  timer.tregister(iTimeout, adapter);
}

void TimedDoor::lock() {
  isOpened = false;
}

int TimedDoor::getTimeOut() const {
  return iTimeout;
}

void TimedDoor::throwState() {
  throw std::runtime_error("Door is still opened after timeout!");
}

void Timer::sleep(int milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void Timer::tregister(int milliseconds, TimerClient* client) {
  this->client = client;
  sleep(milliseconds);
  client->Timeout();
}
