// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <mutex>
#include <unordered_map>

// Структура для хранения состояния двери (поколение и флаг просрочки)
struct DoorState {
  int generation;
  bool expired;
};

// Глобальная карта и мьютекс для синхронизации состояния дверей
static std::unordered_map<TimedDoor*, DoorState> doorStateMap;
static std::mutex doorStateMutex;

// Реализация DoorTimerAdapter
DoorTimerAdapter::DoorTimerAdapter(TimedDoor& doorRef) : door(doorRef) {}

void DoorTimerAdapter::Timeout() {
  // Проверяем, открыта ли дверь
  if (door.isDoorOpened()) {
    std::lock_guard<std::mutex> lock(doorStateMutex);
    doorStateMap[&door].expired = true;
  }
}

// Реализация TimedDoor
TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
  adapter = new DoorTimerAdapter(*this);
  {
    std::lock_guard<std::mutex> lock(doorStateMutex);
    doorStateMap[this] = { 0, false };
  }
}

bool TimedDoor::isDoorOpened() {
  return isOpened;
}

void TimedDoor::unlock() {
  isOpened = true;
  {
    std::lock_guard<std::mutex> lock(doorStateMutex);
    doorStateMap[this].generation++;
    doorStateMap[this].expired = false;
  }
  Timer timer;
  timer.tregister(iTimeout, adapter);
}

void TimedDoor::lock() {
  isOpened = false;
  std::lock_guard<std::mutex> lock(doorStateMutex);
  doorStateMap[this].expired = false;
}

int TimedDoor::getTimeOut() const {
  return iTimeout;
}

void TimedDoor::throwState() {
  std::lock_guard<std::mutex> lock(doorStateMutex);
  if (doorStateMap[this].expired) {
    throw std::runtime_error("Ошибка: дверь открыта слишком долго!");
  }
}

// Реализация Timer
void Timer::sleep(int seconds) {
  std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void Timer::tregister(int timeout, TimerClient* client) {
  DoorTimerAdapter* dta = dynamic_cast<DoorTimerAdapter*>(client);
  if (dta != nullptr) {
    TimedDoor* door = &dta->door;
    int expectedGen;
    {
      std::lock_guard<std::mutex> lock(doorStateMutex);
      expectedGen = doorStateMap[door].generation;
    }
    std::thread([timeout, client, door, expectedGen]() {
      std::this_thread::sleep_for(std::chrono::seconds(timeout));
      int currentGen;
      {
        std::lock_guard<std::mutex> lock(doorStateMutex);
        currentGen = doorStateMap[door].generation;
      }
      if (currentGen == expectedGen && door->isDoorOpened()) {
        client->Timeout();
      }
      }).detach();
  } else {
    std::thread([timeout, client]() {
      std::this_thread::sleep_for(std::chrono::seconds(timeout));
      client->Timeout();
      }).detach();
  }
}
