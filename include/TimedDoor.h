// Copyright 2021 GHA Test Team
#ifndef INCLUDE_TIMEDDOOR_H_
#define INCLUDE_TIMEDDOOR_H_
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
  virtual ~Door() = default;
};

class DoorTimerAdapter : public TimerClient {
 private:
  TimedDoor& door;
 public:
  explicit DoorTimerAdapter(TimedDoor&);
  void Timeout();
};

class TimedDoor : public Door {
 public:
  explicit TimedDoor(int timeout);
  ~TimedDoor() override;
  void unlock() override;
  void lock() override;
  bool isDoorOpened() override;
  void throwState();
  int getTimeout() const { return iTimeout; }
 private:
  int iTimeout;
  bool isOpened;
  DoorTimerAdapter* adapter;
};

class Timer {
  TimerClient *client;
  void sleep(int);
 public:
  void tregister(int, TimerClient*);
};
#endif  // INCLUDE_TIMEDDOOR_H_
