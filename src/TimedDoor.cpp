// Copyright 2021 GHA Test Team
#include <chrono>
#include <stdexcept>
#include <thread>

#include "TimedDoor.h"

DoorTimerAdapter::DoorTimerAdapter(TimedDoor &d) : door(d) {}

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
  if (isOpened) {
    throw std::runtime_error("Door is still open");
  }
}

void Timer::sleep(int time) {
  std::this_thread::sleep_for(std::chrono::seconds(time));
}

void Timer::tregister(int time, TimerClient *cl) {
  client = cl;
  sleep(time);

  if (client != nullptr) {
    client->Timeout();
  }
}
