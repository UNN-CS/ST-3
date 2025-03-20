// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <thread>
#include <chrono>
#include <stdexcept>
#include <iostream>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor &door) : door(door) {}

void DoorTimerAdapter::Timeout()
{
  if (door.isDoorOpened())
  {
    throw std::runtime_error("Door is still open!");
  }
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false)
{
  adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened()
{
  return isOpened;
}

void TimedDoor::unlock()
{
  isOpened = true;
  std::cout << "Door is unlocked." << std::endl;
  // Simulate the timeout mechanism
  Timer timer;
  timer.tregister(iTimeout, adapter);
}

void TimedDoor::lock()
{
  isOpened = false;
  std::cout << "Door is locked." << std::endl;
}

int TimedDoor::getTimeOut() const
{
  return iTimeout;
}

void TimedDoor::throwState()
{
  if (isOpened)
  {
    throw std::runtime_error("Door is open!");
  }
}

void Timer::tregister(int milliseconds, TimerClient *client)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
  client->Timeout();
}
