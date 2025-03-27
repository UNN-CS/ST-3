// Copyright 2025 Konkov Ivan

#ifndef TIMEDDOOR_H
#define TIMEDDOOR_H

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

class TimedDoor;

class DoorTimerAdapter : public TimerClient {
private:
    TimedDoor& door;
public:
    explicit DoorTimerAdapter(TimedDoor&);
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
};

class Timer {
private:
    TimerClient* client;
    void sleep(int);
public:
    void tregister(int, TimerClient*);
};

#endif // TIMEDDOOR_H
