// Copyright 2025 Durynichev Dmitriy

#include "TimedDoor.h"
#include <chrono>
#include <stdexcept>
#include <thread>
#include <memory>
#include <iostream>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor &timedDoor) : door(timedDoor) {}

void DoorTimerAdapter::Timeout() {
  if (door.isDoorOpened()) {
    door.throwState();
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
  auto timer = std::make_shared<Timer>();
  timer->tregister(iTimeout, adapter);
}

void TimedDoor::lock() {
  isOpened = false;
}

TimedDoor::~TimedDoor() {
  delete adapter;
}

void TimedDoor::throwState() {
  if (isOpened) {
    throw std::runtime_error("Door has been open for too long!");
  }
}

bool TimedDoor::isDoorOpened() {
  return isOpened;
}

void Timer::tregister(int seconds, TimerClient *timerClient) {
  client = timerClient;
  std::thread timer_thread([seconds, timerClient]() {
  if (seconds > 0) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
  }

  try {
  if (timerClient) {
    timerClient->Timeout();
  }
  } catch (const std::exception& e) {
    std::cerr << "Exception in Timer thread: " << e.what() << std::endl;
  }
});

  timer_thread.detach();
}

void Timer::sleep(int seconds) {
  if (seconds > 0) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
  }
}
