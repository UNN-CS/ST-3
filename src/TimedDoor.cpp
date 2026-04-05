// Copyright 2021 GHA Test Team

#include "../include/TimedDoor.h"
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor* d)
    : door_(d), cancelled_(std::make_shared<std::atomic<bool> >(false)) {}

void DoorTimerAdapter::Timeout() {
  if (!(*cancelled_)) {
    door_->throwState();
  }
}

void DoorTimerAdapter::cancel() {
  *cancelled_ = true;
}

TimedDoor::TimedDoor(int timeout)
    : adapter_(nullptr),
      iTimeout_(timeout),
      isOpened_(false) {
  adapter_ = new DoorTimerAdapter(this);
}

TimedDoor::~TimedDoor() {
  if (timerThread_.joinable()) {
    if (currentCancelled_) {
      *currentCancelled_ = true;
    }
    timerThread_.join();
  }
  delete adapter_;
}

bool TimedDoor::isDoorOpened() {
  return isOpened_;
}

void TimedDoor::unlock() {
  isOpened_ = true;

  if (timerThread_.joinable()) {
    if (currentCancelled_) {
      *currentCancelled_ = true;
    }
    timerThread_.join();
  }

  delete adapter_;
  adapter_ = new DoorTimerAdapter(this);
  currentCancelled_ = adapter_->getCancelled();

  timerThread_ = std::thread([this]() {
    Timer::tregister(iTimeout_, adapter_);
  });
}

void TimedDoor::lock() {
  isOpened_ = false;
  if (currentCancelled_) {
    *currentCancelled_ = true;
  }
  if (timerThread_.joinable()) {
    timerThread_.join();
  }
}

int TimedDoor::getTimeOut() const {
  return iTimeout_;
}

void TimedDoor::throwState() {
  if (isOpened_) {
    throw std::runtime_error("Door is opened too long!");
  }
}

void Timer::tregister(int timeout, TimerClient* client) {
  if (client) {
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
    client->Timeout();
  }
}
