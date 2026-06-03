// Copyright 2024 Nazyrov A.A.

#include "TimedDoor.h"
#include <thread>
#include <chrono>
#include <stdexcept>

// DoorTimerAdapter implementation
DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door_(door) {}

void DoorTimerAdapter::Timeout() {
  if (door_.IsDoorOpened()) {
    door_.ThrowState();
  }
}

// TimedDoor implementation
TimedDoor::TimedDoor(int timeout)
    : timeout_seconds_(timeout), is_opened_(false) {
  adapter_ = new DoorTimerAdapter(*this);
}

TimedDoor::~TimedDoor() {
  delete adapter_;
}

bool TimedDoor::IsDoorOpened() {
  return is_opened_;
}

void TimedDoor::Unlock() {
  is_opened_ = true;
  Timer timer;
  timer.TRegister(timeout_seconds_, adapter_);
}

void TimedDoor::Lock() {
  is_opened_ = false;
}

int TimedDoor::GetTimeOut() const {
  return timeout_seconds_;
}

void TimedDoor::ThrowState() {
  throw std::runtime_error("Door was left open too long!");
}

// Timer implementation
Timer::Timer() : client_(nullptr) {}

void Timer::Sleep(int seconds) {
  std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void Timer::TRegister(int timeout, TimerClient* client) {
  client_ = client;
  Sleep(timeout);
  if (client_) {
    client_->Timeout();
  }
}
