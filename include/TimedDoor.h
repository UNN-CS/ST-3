// Copyright 2021 GHA Test Team

#ifndef INCLUDE_TIMEDDOOR_H_
#define INCLUDE_TIMEDDOOR_H_

#include <stdexcept>

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
  explicit DoorTimerAdapter(TimedDoor& d);
  void Timeout() override;
};

class TimedDoor : public Door {
 private:
  DoorTimerAdapter* adapter;
  int iTimeout;
  bool isOpened;
  bool alarmArmed;

 public:
  explicit TimedDoor(int timeoutSeconds);
  ~TimedDoor();

  bool isDoorOpened() override;
  void unlock() override;  // open + arm
  void lock() override;    // close + disarm
  int getTimeOut() const;
  bool isAlarmArmed() const;

  void throwState();

  DoorTimerAdapter* getAdapter();
};

class Timer {
 private:
  TimerClient* client;
  void sleep(int seconds);

 public:
  Timer();
  void tregister(int timeout, TimerClient* c);
};

#endif  // INCLUDE_TIMEDDOOR_H_
