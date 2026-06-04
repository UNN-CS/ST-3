// Copyright 2021 GHA Test Team
#include "TimedDoor.h"

#include <chrono>
#include <stdexcept>
#include <thread>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() { door.throwState(); }

TimedDoor::TimedDoor(int timeoutSeconds)
    : iTimeout(timeoutSeconds), isOpened(false), alarmArmed(false) {
  adapter = new DoorTimerAdapter(*this);
}

TimedDoor::~TimedDoor() { delete adapter; }

bool TimedDoor::isDoorOpened() { return isOpened; }

void TimedDoor::unlock() {
  isOpened = true;
  alarmArmed = true;
}

void TimedDoor::lock() {
  isOpened = false;
  alarmArmed = false;
}

int TimedDoor::getTimeOut() const { return iTimeout; }

bool TimedDoor::isAlarmArmed() const { return alarmArmed; }

void TimedDoor::throwState() {
  if (isOpened) {
    throw std::runtime_error(
        "ALARM: refrigerator door left open beyond allowed time!");
  }
}

DoorTimerAdapter* TimedDoor::getAdapter() { return adapter; }

Timer::Timer() : client(nullptr) {}

void Timer::sleep(int seconds) {
  std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void Timer::tregister(int timeout, TimerClient* c) {
  client = c;
  sleep(timeout);
  if (client) {
    client->Timeout();
  }
}
