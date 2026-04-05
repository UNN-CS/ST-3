// include/TimedDoor.h
#ifndef TIMEDDOOR_H
#define TIMEDDOOR_H

class TimedDoor;  // Forward declaration

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
    int getTimeOut() const;
    void throwState();
    DoorTimerAdapter* getAdapter() const { return adapter; }
};

class Timer {
    TimerClient* client;
    void sleep(int);
public:
    void tregister(int, TimerClient*);
};

#endif