// Copyright 2025 tyurinm
#include "TimedDoor.h"
#include <stdexcept>
#include <thread>
#include <memory>
#include <chrono>
#include <iostream>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
  if (door.isDoorOpened()) {
    door.throwState();
  }
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
  adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() {
  return isOpened;
}

void TimedDoor::unlock() {
  isOpened = true;
  if (adapter != nullptr) {
    delete adapter;
  }
  adapter = new DoorTimerAdapter(*this);
  Timer timer;
  timer.tregister(iTimeout, adapter);
}

void TimedDoor::lock() {
  isOpened = false;
  if (adapter != nullptr) {
    delete adapter;
    adapter = nullptr;
  }
}

int TimedDoor::getTimeOut() const {
  return iTimeout;
}

void TimedDoor::throwState() {
  throw std::runtime_error(
    "Warning: The door has remained open longer than permitted!");
}

void Timer::sleep(int timeoutSeconds) {
  if (timeoutSeconds <= 0) {
    throw std::invalid_argument("Error: Timer duration must be positive.");
  }

  std::this_thread::sleep_for(std::chrono::seconds(timeoutSeconds));
}

void Timer::tregister(int timeoutSeconds, TimerClient* client) {
  if (!client) {
    throw std::invalid_argument(
      "Critical: No valid client provided for timer registration.");
  }
  if (timeoutSeconds <= 0) {
    throw std::invalid_argument(
      "Invalid input: Timer duration must be a positive value.");
  }

  this->client = client;

  auto sharedTimer = std::make_shared<Timer>(*this);

  std::thread([sharedTimer, timeoutSeconds]() {
    try {
      sharedTimer->sleep(timeoutSeconds);
      if (sharedTimer->client) {
        sharedTimer->client->Timeout();
      }
    } catch (const std::exception& e) {
      std::cerr << "Timer error: " << e.what() << std::endl;
    }
  }).detach();
}
