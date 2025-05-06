// Copyright 2021 GHA Test Team

#ifndef INCLUDE_TIMEDDOOR_H_
#define INCLUDE_TIMEDDOOR_H_

class DoorTimerAdapter;
class Timer;
class Door;
class TimedDoor;

class TimerClient {
public:
    virtual ~TimerClient() = default;

    virtual void Timeout() = 0;
};

class DoorTimerAdapter final : public TimerClient {
public:
    explicit DoorTimerAdapter(TimedDoor &);

    void Timeout() override;

private:
    TimedDoor &door;
};

class Door {
public:
    virtual ~Door() = default;

    virtual void lock() = 0;

    virtual void unlock() = 0;

    virtual bool isDoorOpened() = 0;
};


class TimedDoor final : public Door {
public:
    explicit TimedDoor(int timeout);

    ~TimedDoor() override;

    void unlock() override;

    void lock() override;

    bool isDoorOpened() override;

    void throwState();

    int getTimeout();

private:
    int iTimeout;
    bool isOpened;
    DoorTimerAdapter *adapter;
};

class Timer {
public:
    void tregister(int, TimerClient *);

private:
    TimerClient *client = nullptr;

    static void sleep(int);
};

#endif  // INCLUDE_TIMEDDOOR_H_
