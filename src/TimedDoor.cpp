// src/TimedDoor.cpp
// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor &d) : door(d) {}

void DoorTimerAdapter::Timeout() {
  if (door.isDoorOpened()) {
    try {
      door.throwState();
    } catch (const std::runtime_error& e) {
      std::cerr << "Ошибка: " << e.what() << std::endl;
    }
  }
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
  adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() {
  return isOpened;
}

void TimedDoor::unlock() {
  std::cout << "Дверь открыта" << std::endl;
  isOpened = true;
  Timer timer;
  timer.tregister(iTimeout, adapter);
}

void TimedDoor::lock() {
  std::cout << "Дверь закрыта" << std::endl;
  isOpened = false;
}

int TimedDoor::getTimeOut() const {
  return iTimeout;
}

void TimedDoor::throwState() {
  throw std::runtime_error("Дверь остается открытой после истечения времени");
}

void Timer::tregister(int timeout, TimerClient* client) {
  std::thread([timeout, client]() {
    std::this_thread::sleep_for(std::chrono::seconds(timeout));
    if (client) {
      client->Timeout();
    }
  }).detach();
}
