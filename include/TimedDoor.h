// Copyright 2021 GHA Test Team

#ifndef INCLUDE_TIMEDDOOR_H_
#define INCLUDE_TIMEDDOOR_H_

class TimedDoor;

class TimerClient {
 public:
    virtual void Timeout() = 0;
    virtual ~TimerClient() {}
};

class Door {
 public:
    virtual void lock() = 0;
    virtual void unlock() = 0;
    virtual bool isDoorOpened() = 0;
    virtual ~Door() {}
};

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
    explicit TimedDoor(int);
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
