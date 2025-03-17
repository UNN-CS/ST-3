// Copyright 2021 GHA Test Team
#include "../include/TimedDoor.h"
#include <chrono>
#include <stdexcept>
#include <thread>
#include "TimedDoor.h"


TimedDoor::TimedDoor(int timeout) {
    iTimeout = timeout;
    isOpened = false;
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
void TimedDoor::throwState(){
    if (isOpened) throw std::runtime_error("Door is open for too long");
}

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout()
{
    door.throwState();
}
