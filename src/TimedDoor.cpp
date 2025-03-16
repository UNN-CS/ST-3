// Copyright 2025 ovVrLFg8ks
#include "TimedDoor.h"

#include <chrono>
#include <stdexcept>
#include <thread>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
  door.isDoorOpened() ? door.throwState() : void();
}

TimedDoor::TimedDoor(int iTimeout) : iTimeout(iTimeout) {}

bool TimedDoor::isDoorOpened() { return isOpened; }

void TimedDoor::unlock() {
  isOpened = true;
}

void TimedDoor::lock() { isOpened = false; }

int TimedDoor::getTimeOut() const { return iTimeout; }

void TimedDoor::throwState() {
  throw std::runtime_error("Shut the door dammit");
}

void Timer::sleep(int duration) {
  std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}

void Timer::tregister(int timeout, TimerClient* client) {
  this->client = client;
  this->sleep(timeout);
  client->Timeout();
}
