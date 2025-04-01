// Copyright 2021 GHA Test Team

#include "TimedDoor.h"
#include <memory>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
  door.throwState();
}

class RealTimer : public Timer {
 private:
  TimerClient* client = nullptr;
 public:
  void tregister(int, TimerClient* client) override {
    this->client = client;
  }
  void trigger() override {
    if (client) client->Timeout();
  }
};

TimedDoor::TimedDoor(int timeout) 
  : iTimeout(timeout), isOpened(false), timer(new RealTimer()) {
  adapter = new DoorTimerAdapter(*this);
}

TimedDoor::~TimedDoor() {
  delete adapter;
  delete timer;
}

bool TimedDoor::isDoorOpened() {
  return isOpened;
}

void TimedDoor::unlock() {
  isOpened = true;
  timer->tregister(iTimeout, adapter);
}

void TimedDoor::lock() {
  isOpened = false;
}

int TimedDoor::getTimeOut() const {
  return iTimeout;
}

void TimedDoor::throwState() {
  if (isOpened) {
    throw std::runtime_error("Door is still open!");
  }
}

Timer* TimedDoor::getTimer() {
  return timer;
}
