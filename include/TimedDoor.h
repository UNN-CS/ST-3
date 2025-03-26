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

class Door {
 public:
  virtual ~Door() = default;
  virtual void lock() = 0;
  virtual void unlock() = 0;
  virtual bool isDoorOpened() = 0;
};

class DoorTimerAdapter : public TimerClient {
 private:
  TimedDoor& door;
 public:
  explicit DoorTimerAdapter(TimedDoor&);
  void Timeout();
};

class TimedDoor : public Door {
 private:
  Timer& timer;
  DoorTimerAdapter* adapter;
  int iTimeout;
  bool isOpened;
 public:
  explicit TimedDoor(int timeout, Timer& t);
  ~TimedDoor() override;
  bool isDoorOpened() override;
  void unlock() override;
  void lock() override;
  int getTimeOut() const;
  void throwState();
};

class Timer {
 public:
  virtual ~Timer() = default;
  virtual void tregister(int timeout, TimerClient* client) = 0;
  virtual void sleep(int ms) = 0;
};

#endif  // INCLUDE_TIMEDDOOR_H_
