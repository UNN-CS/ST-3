// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <stdexcept>
#include <thread>
#include <chrono>

class DoorTimerAdapter : public TimerClient {
private:
    TimedDoor& doorRef;
public:
    explicit DoorTimerAdapter(TimedDoor& door)
        : doorRef(door) {}

    void Timeout() override {
        doorRef.throwState();
    }
};

TimedDoor::TimedDoor(int timeout)
    : iTimeout(timeout), isOpened(false) {
    adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    isOpened = true;
    Timer t;
    t.tregister(iTimeout, adapter);
}

void TimedDoor::lock() {
    isOpened = false;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    if (isOpened) {
        throw std::runtime_error("Door left open");
    }
}

void Timer::sleep(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void Timer::tregister(int seconds, TimerClient* client) {
    this->client = client;
    sleep(seconds);
    try {
        client->Timeout();
    }
    catch (...) {
    }
}
