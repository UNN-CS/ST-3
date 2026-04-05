// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <chrono>
#include <stdexcept>
#include <thread>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d)
    : door(d), inTimerCallback_(false) {}

void DoorTimerAdapter::Timeout() {
  if (!inTimerCallback_) {
    inTimerCallback_ = true;
    Timer timer;
    timer.tregister(door.getTimeOut(), this);
    inTimerCallback_ = false;
  } else {
    if (door.isDoorOpened()) {
      door.throwState();
    }
  }
}

TimedDoor::TimedDoor(int timeoutMs)
    : adapter(new DoorTimerAdapter(*this)),
      iTimeout(timeoutMs),
      isOpened(false) {}

TimedDoor::~TimedDoor() {
  delete adapter;
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
  throw std::runtime_error("Door has been open too long");
}

void Timer::sleep(int milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void Timer::tregister(int timeout, TimerClient* c) {
  client = c;
  sleep(timeout);
  if (client != nullptr) {
    client->Timeout();
  }
}
