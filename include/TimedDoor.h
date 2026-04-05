// Copyright 2021 GHA Test Team

#ifndef INCLUDE_TIMEDDOOR_H_
#define INCLUDE_TIMEDDOOR_H_

#include <mutex>

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
  virtual bool isDoorOpened() const = 0;
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
  DoorTimerAdapter * adapter;
  int iTimeout;
  bool isOpened;
  mutable std::mutex mutex_;
  Timer *timerForTest_;

 public:
  explicit TimedDoor(int);
  ~TimedDoor();
  bool isDoorOpened() const override;
  void unlock() override;
  void lock() override;
  int getTimeOut() const;
  void throwState();
  void setTimerForTest(Timer *timer);
};

class Timer {
 protected:
  virtual void sleep(int delay);
  TimerClient *client;

 public:
  virtual ~Timer() = default;
  virtual void tregister(int delay, TimerClient *timerClient);
};

#endif  // INCLUDE_TIMEDDOOR_H_
