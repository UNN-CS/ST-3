// Copyright 2025 Kapustin Ivan
#include "TimedDoor.h"
#include <stdexcept>
#include <iostream>
#include <thread>
#include <chrono>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
  adapter = new DoorTimerAdapter(*this);
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
  if (isOpened) {
    throw std::runtime_error("Door is open!");
  }
}

void DoorTimerAdapter::Timeout() {
  door.throwState();
}

void Timer::sleep(int timeout) {
  std::this_thread::sleep_for(std::chrono::seconds(timeout));
}

void Timer::tregister(int timeout, TimerClient* client) {
  this->client = client;
  if (client) {
    sleep(timeout);
    client->Timeout();
  } else {
    std::cout << "No client provided, skipping timeout." << std::endl;
  }
}
