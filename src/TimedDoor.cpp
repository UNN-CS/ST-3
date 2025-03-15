// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <chrono>
#include <stdexcept>
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
  throw std::runtime_error("Door has been open for too long!");
}

void Timer::sleep(int seconds) {
  if (seconds > 0) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
  }
}

void Timer::tregister(int seconds, TimerClient *timerClient) {
  client = timerClient;

  std::thread timer_thread([seconds, timerClient]() {
    if (seconds > 0) {
      std::this_thread::sleep_for(std::chrono::seconds(seconds));
    }

    if (timerClient) {
      timerClient->Timeout();
    }
  });

  timer_thread.detach();
}
