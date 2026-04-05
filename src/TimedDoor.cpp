// Copyright 2021 GHA Test Team

#include "TimedDoor.h"

#include <chrono>
#include <stdexcept>
#include <thread>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
  door.throwState();
}

TimedDoor::TimedDoor(int timeout)
    : adapter(new DoorTimerAdapter(*this)),
      iTimeout(timeout),
      isOpened(false),
      timerForTest_(nullptr) {}

TimedDoor::~TimedDoor() {
  delete adapter;
}

bool TimedDoor::isDoorOpened() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return isOpened;
}

void TimedDoor::lock() {
  std::lock_guard<std::mutex> lock(mutex_);
  isOpened = false;
}

void TimedDoor::unlock() {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    isOpened = true;
  }
  Timer stackTimer;
  Timer *timer = timerForTest_ != nullptr ? timerForTest_ : &stackTimer;
  timer->tregister(iTimeout, adapter);
}

int TimedDoor::getTimeOut() const {
  return iTimeout;
}

void TimedDoor::throwState() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (isOpened) {
    throw std::runtime_error("Door left open too long");
  }
}

void TimedDoor::setTimerForTest(Timer *timer) {
  timerForTest_ = timer;
}

void Timer::sleep(int delay) {
  if (delay > 0) {
    std::this_thread::sleep_for(std::chrono::seconds(delay));
  }
}

void Timer::tregister(int delay, TimerClient *timerClient) {
  client = timerClient;
  sleep(delay);
  if (client != nullptr) {
    client->Timeout();
  }
}
