// Copyright 2021 GHA Test Team

#ifndef INCLUDE_TIMEDDOOR_H_
#define INCLUDE_TIMEDDOOR_H_

#include <stdexcept>
#include <thread>
#include <exception>

class DoorTimerAdapter;
class Timer;
class Door;
class TimedDoor;

class TimerClient {
public:
    virtual void Timeout() = 0;
};

class Door {
public:
    virtual void lock() = 0;
    virtual void unlock() = 0;
    virtual bool isDoorOpened() = 0;
};

class DoorTimerAdapter : public TimerClient {
    // private:
    TimedDoor& door;
public:
    explicit DoorTimerAdapter(TimedDoor&);
    void Timeout();
};

class TimedDoor : public Door {
    // private:
    DoorTimerAdapter* adapter;
    int iTimeout;
    bool isOpened;
    std::thread timerThread;
    std::exception_ptr timerException;
public:
    explicit TimedDoor(int);
    ~TimedDoor();
    bool isDoorOpened();
    void unlock();
    void lock();
    int getTimeOut() const;
    void throwState();
    void joinTimerThread();
};

class Timer {
    TimerClient* client;
    void sleep(int);
public:
    void tregister(int, TimerClient*);
};


#endif  // INCLUDE_TIMEDDOOR_H_
