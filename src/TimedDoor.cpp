// Copyright 2021 GHA Test Team

#include "TimedDoor.h"

#include <chrono>
#include <thread>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor &d) : door(d) {}

void DoorTimerAdapter::Timeout() {
  if (door.isDoorOpened()) {
    door.throwState();
  }
}

Timer::Timer() : client(nullptr) {}

void Timer::sleep(int milliseconds) {
  if (milliseconds < 0) {
    throw std::invalid_argument("Timer timeout cannot be negative");
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void Timer::tregister(int timeout, TimerClient *timerClient) {
  client = timerClient;
  sleep(timeout);

  if (client != nullptr) {
    client->Timeout();
  }
}

TimedDoor::TimedDoor(int timeout)
    : adapter(*this), timer(), iTimeout(timeout), isOpened(false) {}

bool TimedDoor::isDoorOpened() { return isOpened; }

void TimedDoor::unlock() {
  isOpened = true;
  timer.tregister(iTimeout, &adapter);
}

void TimedDoor::lock() { isOpened = false; }

int TimedDoor::getTimeOut() const { return iTimeout; }

void TimedDoor::throwState() {
  throw DoorTimeoutException("Door remained open past timeout");
}

Timer *TimedDoor::getTimer() { return &timer; }

void TimedDoor::triggerTimeoutForTest() { adapter.Timeout(); }

void TimedDoor::registerTimerForTest(int timeout, TimerClient *client) {
  timer.tregister(timeout, client);
}
