// Copyright 2021 GHA Test Team
#include <chrono>
#include <stdexcept>
#include <thread>

#include "TimedDoor.h"

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& _door) : door(_door) {}

void DoorTimerAdapter::Timeout() {
  if (door.isDoorOpened()) {
    throw std::runtime_error("The door has been open for a long time");
  }
}

TimedDoor::TimedDoor(int _timeout)
  : adapter(new DoorTimerAdapter(*this)), iTimeout(_timeout > 0 ? _timeout : 1), isOpened(false) {}

bool TimedDoor::isDoorOpened() { return isOpened; }

void TimedDoor::unlock() {
  if (isOpened) {
    throw std::runtime_error("Door is already opened");
  }
  isOpened = true;
  Timer t;
  t.tregister(iTimeout, adapter);
}

void TimedDoor::lock() {
  if (isOpened) {
    throw std::runtime_error("Door is already closed");
  }
  isOpened = false;
}

int TimedDoor::getTimeOut() const { return iTimeout; }

void TimedDoor::throwState() {
  if (isOpened) {
    throw std::runtime_error("Door is opened");
  }
  else {
    throw std::runtime_error("Door is closed");
  }
}

void Timer::sleep(int _timeout) { std::this_thread::sleep_for(std::chrono::seconds(_timeout)); }

void Timer::tregister(int _timeout, TimerClient* _client) {
  if (_client == nullptr) {
    throw std::runtime_error("Client is null");
  }
  if (_timeout < 0) {
    throw std::runtime_error("Timeout is negative");
  }
  client = _client;
  sleep(_timeout);
  client->Timeout();
}