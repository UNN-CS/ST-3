// Copyright 2021 GHA Test Team
#include "TimedDoor.h"

#include <algorithm>
#include <chrono>
#include <mutex>
#include <stdexcept>
#include <thread>

namespace {

constexpr char kDoorOpenMessage[] = "Door is opened";

std::mutex& timedDoorMutex() {
  static std::mutex mutex;
  return mutex;
}

}  // namespace

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door_) : door(door_) {}

void DoorTimerAdapter::Timeout() {
  door.throwState();
}

TimedDoor::TimedDoor(int timeout)
    : adapter(new DoorTimerAdapter(*this)), iTimeout(timeout), isOpened(false) {
  lock();
}

bool TimedDoor::isDoorOpened() {
  std::lock_guard<std::mutex> lock(timedDoorMutex());
  return isOpened;
}

void TimedDoor::unlock() {
  {
    std::lock_guard<std::mutex> lock(timedDoorMutex());
    isOpened = true;
  }

  Timer timer;
  timer.tregister(getTimeOut(), adapter);
}

void TimedDoor::lock() {
  std::lock_guard<std::mutex> lock(timedDoorMutex());
  isOpened = false;
}

int TimedDoor::getTimeOut() const {
  return iTimeout;
}

void TimedDoor::throwState() {
  if (isDoorOpened()) {
    throw std::runtime_error(kDoorOpenMessage);
  }
}

void Timer::sleep(int timeout) {
  std::this_thread::sleep_for(std::chrono::seconds(std::max(timeout, 0)));
}

void Timer::tregister(int timeout, TimerClient* client_) {
  client = client_;
  sleep(timeout);

  if (client != nullptr) {
    client->Timeout();
  }
}
