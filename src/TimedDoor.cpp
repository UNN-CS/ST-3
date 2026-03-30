// Copyright 2026 GHA Test Team

#include "TimedDoor.h"

#include <chrono>
#include <stdexcept>
#include <thread>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor &d) : door(d) {}

void DoorTimerAdapter::Timeout() {
  door.throwState();
}

TimedDoor::TimedDoor(int timeout)
    : iTimeout(timeout), isOpened(false), timerActive(false) {
  adapter = new DoorTimerAdapter(*this);
}

TimedDoor::~TimedDoor() {
  delete adapter;
}

bool TimedDoor::isDoorOpened() {
  return isOpened;
}

void TimedDoor::unlock() {
  if (!isOpened) {
    isOpened = true;
    timerActive = true;
    timerThread = std::thread([this]() {
      std::this_thread::sleep_for(std::chrono::seconds(iTimeout));
      if (timerActive && isOpened) {
        adapter->Timeout();
      }
    });
  }
}

void TimedDoor::lock() {
  if (isOpened) {
    isOpened = false;
    timerActive = false;
    if (timerThread.joinable()) {
      timerThread.detach();
    }
  }
}

int TimedDoor::getTimeOut() const {
  return iTimeout;
}

void TimedDoor::throwState() {
  if (isOpened) {
    throw std::runtime_error("Door timeout: door was left open too long!");
  }
}

void Timer::sleep(int seconds) {
  std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void Timer::tregister(int timeout, TimerClient *client) {
  std::thread([timeout, client]() {
    std::this_thread::sleep_for(std::chrono::seconds(timeout));
    client->Timeout();
  }).detach();
}
