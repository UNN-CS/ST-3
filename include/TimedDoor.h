// Copyright 2025 Konkov Ivan

#pragma once
#include <exception>
#include <memory>

class TimerClient {
public:
    virtual void Timeout() = 0;
    virtual ~TimerClient() = default;
};

class Door {
public:
    virtual void lock() = 0;
    virtual void unlock() = 0;
    virtual bool isDoorOpened() = 0;
    virtual ~Door() = default;
};

class TimedDoor;

class DoorTimerAdapter : public TimerClient {
private:
    TimedDoor& door;
public:
    explicit DoorTimerAdapter(TimedDoor& door);
    void Timeout() override;
};

class TimedDoor : public Door {
private:
    DoorTimerAdapter* adapter;
    int iTimeout;
    bool isOpened;
public:
    explicit TimedDoor(int timeout);
    ~TimedDoor();
    
    bool isDoorOpened() override;
    void unlock() override;
    void lock() override;
    
    int getTimeOut();
    void throwState();
    DoorTimerAdapter* getAdapter();
};

class Timer {
public:
    void tregister(int timeout, TimerClient* client);
};
