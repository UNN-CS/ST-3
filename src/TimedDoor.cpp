// Copyright 2021 GHA Test Team
#include "TimedDoor.h"

#include <chrono>
#include <stdexcept>
#include <thread>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& timedDoor) : door(timedDoor) {}

void DoorTimerAdapter::Timeout() {
  if (door.isDoorOpened()) {
    door.throwState();
  }
}

TimedDoor::TimedDoor(int timeout)
    : adapter(new DoorTimerAdapter(*this)),
      iTimeout(timeout),
      isOpened(false) {}

TimedDoor::~TimedDoor() {
  delete adapter;
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
  throw std::runtime_error("Door timeout exceeded");
}

void Timer::sleep(int timeout) {
  if (timeout > 0) {
    std::this_thread::sleep_for(std::chrono::seconds(timeout));
  }
}

void Timer::tregister(int timeout, TimerClient* timerClient) {
  client = timerClient;
  sleep(timeout);
  if (client != nullptr) {
    client->Timeout();
  }
}
