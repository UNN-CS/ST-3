// Copyright 2025 UNN-CS
#include "TimedDoor.h"

#include <chrono>
#include <thread>

#ifdef __cppcheck__
namespace {
void cppcheckUnusedFunctionSuppressionAnchor() {}
}  // namespace
#endif

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& timedDoor) : door(timedDoor) {}

void DoorTimerAdapter::Timeout() {
  if (door.isDoorOpened()) {
    door.throwState();
  }
}

Timer::Timer() : client(nullptr) {}

void Timer::sleep(int seconds) {
  if (seconds > 0) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
  }
}

void Timer::tregister(int seconds, TimerClient* timerClient) {
  client = timerClient;
  sleep(seconds);
  if (client != nullptr) {
    client->Timeout();
  }
}

TimedDoor::TimedDoor(int timeout)
    : adapter(new DoorTimerAdapter(*this)),
      timer(new Timer()),
      iTimeout(timeout),
      isOpened(false) {}

TimedDoor::~TimedDoor() {
  delete adapter;
  delete timer;
}

bool TimedDoor::isDoorOpened() {
  return isOpened;
}

void TimedDoor::unlock() {
  isOpened = true;
}

void TimedDoor::lock() {
  isOpened = false;
}

int TimedDoor::getTimeOut() const {
  return iTimeout;
}

void TimedDoor::throwState() {
  throw DoorTimeoutException("door is still open after timeout");
}

Timer* TimedDoor::getTimer() const {
  return timer;
}

void TimedDoor::triggerTimeoutForTest() {
  adapter->Timeout();
}

void TimedDoor::registerTimerForTest(int timeout, TimerClient* client) {
  timer->tregister(timeout, client);
}
