// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <stdexcept>
#include <thread>

// DoorTimerAdapter implementation
DoorTimerAdapter::DoorTimerAdapter(TimedDoor &d) : door(d) {}

void DoorTimerAdapter::Timeout() { door.throwState(); }

// TimedDoor implementation
bool TimedDoor::isDoorOpened() { return isOpened; }

void TimedDoor::unlock() {
  isOpened = true;
  extern Timer timer;
  timer.tregister(iTimeout, adapter);
}

void TimedDoor::lock() { isOpened = false; }

int TimedDoor::getTimeOut() const { return iTimeout; }

void TimedDoor::throwState() {
  if (isOpened) {
    throw std::runtime_error("Door is still open!");
  }
}

void Timer::tregister(int timeout, TimerClient *client) {
  std::thread([timeout, client]() {
    std::this_thread::sleep_for(std::chrono::seconds(timeout));
    client->Timeout();
  }).detach();
}

// Global Timer instance
Timer timer;
