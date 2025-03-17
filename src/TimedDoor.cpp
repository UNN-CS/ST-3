// Copyright 2021 GHA Test Team

#include "TimedDoor.h"
#include <chrono>
#include <stdexcept>
#include <thread>
#include <memory>
#include <iostream>

DoorTimerAdapter::DoorTimerAdapter(const TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
  if (door.isDoorOpened()) {
    const_cast<TimedDoor&>(door).throwState();
  }
}

TimedDoor::TimedDoor(int timeout) : isOpened(false) {
  if (timeout <= 0) {
    throw std::invalid_argument("Timeout must be greater than 0");
  }
  iTimeout = timeout;
  adapter = new DoorTimerAdapter(*this);
}

void TimedDoor::unlock() {
  if (iTimeout <= 0) {
    throw std::invalid_argument("Timeout must be greater than 0");
  }
  isOpened = true;
  timer.tregister(iTimeout, adapter);
}

void TimedDoor::lock() {
  isOpened = false;
}

TimedDoor::~TimedDoor() {
  delete adapter;
}

void TimedDoor::throwState() {
  if (isOpened) {
    throw std::runtime_error("Door is still opened after timeout!");
  }
}

bool TimedDoor::isDoorOpened() const {
  return isOpened;
}

int TimedDoor::getTimeOut() const {
  return iTimeout;
}

// Реализация Timer
void Timer::sleep(int milliseconds) {
  if (milliseconds > 0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
  }
}

void Timer::tregister(int milliseconds, TimerClient* client) {
  this->client = client;
  std::thread timer_thread([milliseconds, client]() {
    if (milliseconds > 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    try {
      if (client) {
        client->Timeout();
      }
    } catch (const std::exception& e) {
      std::cerr << "Exception in Timer thread: " << e.what() << std::endl;
    }
  });

  timer_thread.detach();
}
