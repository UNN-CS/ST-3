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

class Timer {
 protected:
  TimerClient* client;
  virtual void sleep(int);
 public:
  virtual void tregister(int, TimerClient*);
  virtual ~Timer() = default;
};

class TimedDoor : public Door {
 private:
  DoorTimerAdapter* adapter;
  Timer* timer;
  int iTimeout;
  bool isOpened;
 public:
  explicit TimedDoor(int timeout, Timer* t = nullptr);
  bool isDoorOpened() override;
  void unlock() override;
  void lock() override;
  int getTimeOut() const;
  void throwState();
  ~TimedDoor();
};

#endif  // INCLUDE_TIMEDDOOR_H_
