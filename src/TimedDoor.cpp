// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

void Timer::sleep(int s) {
  if (s <= 0) {
    throw "negative or zero time";
    return;
  }
  std::this_thread::sleep_for(std::chrono::seconds(s));
}
void Timer::tregister(int s, TimerClient *c) {
  if (s <= 0) {
    throw "negative or zero time";
    return;
  }
  if (c == nullptr) {
    throw "no client";
    return;
  }
  client = c;
  sleep(s);

  client->Timeout();
}

void DoorTimerAdapter::Timeout() {
  if (door.isDoorOpened()) {
    throw std::runtime_error("the refrigerator melted");
  }
}
DoorTimerAdapter::DoorTimerAdapter(TimedDoor &door) : door(door) {}

TimedDoor::TimedDoor(int s) {
  adapter = new DoorTimerAdapter(*this);
  iTimeout = s;
  isOpened = false;
}
bool TimedDoor::isDoorOpened() { return isOpened; }
void TimedDoor::unlock() {
  std::cout << "-------------------- :" << isOpened << std::endl;
  if (isOpened) {
    throw "the door hinges are broken";
  }
  isOpened = true;
}
void TimedDoor::lock() {
  if (!isOpened) {
    throw "already closed";
  }
  isOpened = false;
}
int TimedDoor::getTimeOut() const { return iTimeout; }
void TimedDoor::throwState() { throw isOpened ? "open" : "closed"; }
