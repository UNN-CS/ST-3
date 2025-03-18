// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <chrono>
#include <stdexcept>
#include <string>
#include <thread>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor &timedDoor) : door(timedDoor) {}

void DoorTimerAdapter::Timeout() {
  if (door.isDoorOpened()) {
    door.throwState();
  }
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
  adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() { return isOpened; }

void TimedDoor::unlock() {
  isOpened = true;
  Timer timer;
  timer.tregister(iTimeout, adapter);
}

void TimedDoor::lock() { isOpened = false; }

int TimedDoor::getTimeOut() const { return iTimeout; }

void TimedDoor::throwState() {
  throw std::runtime_error("Door open too long...");
}

void Timer::sleep(int seconds) {
  if (seconds > 0) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
  }
}

void Timer::tregister(int timeoutSeconds, TimerClient *client) {
  if (client == nullptr) {
    throw std::invalid_argument("Client null");
  }
  if (timeoutSeconds <= 0) {
    throw std::invalid_argument("Timeout must be > 0");
  }
  this->client = client;
  std::thread([timeoutSeconds, client]() {
    try {
      if (timeoutSeconds > 0) {
        std::this_thread::sleep_for(std::chrono::seconds(timeoutSeconds));
      }
      if (client) {
        client->Timeout();
      }
    } catch (const std::exception &e) {
      throw std::runtime_error(std::string("Timer error: ") + e.what());
    }
  }).detach();
}
