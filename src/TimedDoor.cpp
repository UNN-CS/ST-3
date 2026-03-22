// Copyright 2021 GHA Test Team
#include "TimedDoor.h"

#include <chrono>
#include <thread>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& _door) : door(_door) {}

void DoorTimerAdapter::Timeout() { door.throwState(); }

TimedDoor::TimedDoor(int _timeout) : iTimeout(_timeout), isOpened(false) {
  adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() { return isOpened; }

void TimedDoor::unlock() { isOpened = true; }

void TimedDoor::lock() { isOpened = false; }

int TimedDoor::getTimeOut() const { return iTimeout; }

void TimedDoor::throwState() {
  if (isOpened == true) {
    throw -1;
  }
}

void Timer::sleep(int _time) {
  std::this_thread::sleep_for(std::chrono::seconds(_time));
}

void Timer::tregister(int _timeout, TimerClient* _client) {
  this->client = _client;
  sleep(_timeout);
  client->Timeout();
}
