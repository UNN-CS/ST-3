// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <chrono>
#include <thread>

// Реализация DoorTimerAdapter

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& doorRef) : door(doorRef) {}

void DoorTimerAdapter::Timeout() {
    // Если дверь до сих пор открыта, вызываем выброс исключения
    if (door.isDoorOpened()) {
        door.throwState();
    }
}

// Реализация TimedDoor

TimedDoor::TimedDoor(int timeout)
    : iTimeout(timeout), isOpened(false), timerException(nullptr) {
    adapter = new DoorTimerAdapter(*this);
}

TimedDoor::~TimedDoor() {
    if (timerThread.joinable()) {
        timerThread.join();
    }
    delete adapter;
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    isOpened = true;
    // Запускаем асинхронно таймер
    timerThread = std::thread([this]() {
        Timer timer;
        try {
            timer.tregister(iTimeout, adapter);
        }
        catch (...) {
            timerException = std::current_exception();
        }
        });
}

void TimedDoor::lock() {
    isOpened = false;
    // Если таймер запущен, дожидаемся его завершения
    if (timerThread.joinable()) {
        timerThread.join();
    }
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    throw std::runtime_error("Door left open too long!");
}

void TimedDoor::joinTimerThread() {
    if (timerThread.joinable()) {
        timerThread.join();
    }
    if (timerException) {
        // Сохраняем исключение и сбрасываем флаг, чтобы повторные вызовы не выбрасывали его
        auto ex = timerException;
        timerException = nullptr;
        std::rethrow_exception(ex);
    }
}

// Реализация Timer

void Timer::sleep(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void Timer::tregister(int seconds, TimerClient* client) {
    this->client = client;
    sleep(seconds);
    client->Timeout();
}