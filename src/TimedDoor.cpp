// Copyright 2026 Yakimov
#include "../include/TimedDoor.h"

#include <chrono>
#include <stdexcept>
#include <thread>

void Timer::sleep(int milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void Timer::tregister(int timeout, TimerClient* client) {
  this->client = client;
  sleep(timeout);
  if (client != nullptr) {
    client->Timeout();
  }
}

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& doorRef) : door(doorRef) {}

void DoorTimerAdapter::Timeout() {
  if (door.isDoorOpened()) {
    door.throwState();
  }
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
  adapter = new DoorTimerAdapter(*this);
}

TimedDoor::~TimedDoor() {
  delete adapter;
}

bool TimedDoor::isDoorOpened() {
  return isOpened;
}

void TimedDoor::unlock() {
  isOpened = true;
}

void TimedDoor::lock() {
  isOpened = false;
}

int TimedDoor::getTimeOut() const {
  return iTimeout;
}

void TimedDoor::throwState() {
  throw std::runtime_error("Door has been open for too long!");
}
