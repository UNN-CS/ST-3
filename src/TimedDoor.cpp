// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <stdexcept>
#include <thread>
#include <chrono>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() { door.throwState(); }

TimedDoor::TimedDoor(int t) : adapter(new DoorTimerAdapter(*this)),
                              iTimeout(t),
                              isOpened(false) {}

bool TimedDoor::isDoorOpened() { return isOpened; }

void TimedDoor::unlock() { isOpened = true; }

void TimedDoor::lock() { isOpened = false; }

int TimedDoor::getTimeOut() const { return iTimeout; }

void TimedDoor::throwState() {
    if (isOpened) throw std::runtime_error("door is still open");
}

void Timer::sleep(int t) {
    if (t > 0) std::this_thread::sleep_for(std::chrono::milliseconds(t));
}

void Timer::tregister(int t, TimerClient* c) {
    client = c;
    sleep(t);
    if (client) client->Timeout();
}
