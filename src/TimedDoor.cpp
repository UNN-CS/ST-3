// Copyright 2021 GHA Test Team
#include "TimedDoor.h"

#include <chrono>
#include <stdexcept>
#include <thread>

// delay_ms in tregister / iTimeout: milliseconds (wall clock, best effort).

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
  door.throwState();
}

TimedDoor::TimedDoor(int timeout_ms)
    : adapter(new DoorTimerAdapter(*this)),
      iTimeout(timeout_ms),
      isOpened(false) {}

TimedDoor::~TimedDoor() {
  delete adapter;
}

bool TimedDoor::isDoorOpened() {
  std::lock_guard<std::mutex> lock(mutex_);
  return isOpened;
}

void TimedDoor::unlock() {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    isOpened = true;
  }
  Timer timer;
  timer.tregister(iTimeout, adapter);
}

void TimedDoor::lock() {
  std::lock_guard<std::mutex> lock(mutex_);
  isOpened = false;
}

int TimedDoor::getTimeOut() const {
  return iTimeout;
}

void TimedDoor::throwState() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (isOpened) {
    throw std::runtime_error("TimedDoor: open past timeout");
  }
}

void Timer::sleep(int delay_ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
}

void Timer::tregister(int delay_ms, TimerClient* c) {
  client = c;
  sleep(delay_ms);
  if (client != nullptr) {
    client->Timeout();
  }
}
