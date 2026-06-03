// Copyright 2021 GHA Test Team

#include "TimedDoor.h"

#include <chrono>   // NOLINT
#include <stdexcept>
#include <thread>   // NOLINT

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door_(door) {}

void DoorTimerAdapter::Timeout() {
  door_.throwState();
}

TimedDoor::TimedDoor(int timeout)
    : adapter_(new DoorTimerAdapter(*this)),
      timeout_(timeout),
      opened_(false) {}

TimedDoor::~TimedDoor() {
  delete adapter_;
}

bool TimedDoor::isDoorOpened() {
  return opened_;
}

void TimedDoor::unlock() {
  opened_ = true;
}

void TimedDoor::lock() {
  opened_ = false;
}

int TimedDoor::getTimeOut() const {
  return timeout_;
}

void TimedDoor::throwState() {
  if (opened_) {
    throw std::runtime_error("Door has been open for too long");
  }
}

void Timer::sleep(int milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void Timer::tregister(int timeout, TimerClient* client) {
  client_ = client;
  if (client_ != nullptr) {
    sleep(timeout);
    client_->Timeout();
  }
}
