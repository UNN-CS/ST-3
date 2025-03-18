// Copyright 2021 GHA Test Team
#include "TimedDoor.h"

#include <chrono>
#include <memory>
#include <thread>
#include <utility>

TimedDoor::TimedDoor(int time) : m_timeout(time)
{ m_adapter.SetTimedDoorData(time, weak_from_this()); }

bool TimedDoor::isDoorOpened() const { return m_isOpened; }

void TimedDoor::unlock() {
  if (m_isOpened) {
    throw std::runtime_error("Door is already opened");
  }
  m_isOpened = true;
}

void TimedDoor::lock() {
  if (!m_isOpened) {
    throw std::runtime_error("Door is already closed");
  }
  m_isOpened = false;
}

int TimedDoor::getTimeOut() const { return m_timeout; }

void TimedDoor::throwState() { throw std::runtime_error("Time is out"); }

void DoorTimerAdapter::SetTimedDoorData(int baseSleepTime,
    std::weak_ptr<TimedDoor> door) {
  m_door = std::move(door);
  m_baseSleepTime = baseSleepTime;
}

void DoorTimerAdapter::Timeout()
{ m_timer.tregister(m_baseSleepTime, weak_from_this()); }

void Timer::tregister(int time, std::weak_ptr<TimerClient> client) {
  m_client = std::move(client);
  sleep(time);
}

void Timer::sleep(int time)
{ std::this_thread::sleep_for(std::chrono::seconds(time)); }
