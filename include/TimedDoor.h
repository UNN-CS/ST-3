// Copyright 2021 GHA Test Team
#ifndef INCLUDE_TIMEDDOOR_H_
#define INCLUDE_TIMEDDOOR_H_

#include <stdexcept>
#include <thread>

class TimerClient;
class Door;
class TimedDoor;
class Timer;

class TimerClient {
 public:
  virtual void Timeout() = 0;
};

class Door {
 public:
  virtual void lock() = 0;
  virtual void unlock() = 0;
  virtual bool isDoorOpened() const = 0;
};

class Timer {
  public:
      virtual void tregister(int seconds, TimerClient* timerClient);
      virtual ~Timer() = default;
  private:
      TimerClient* client;
      void sleep(int seconds);
  };

class DoorTimerAdapter : public TimerClient {
 private:
  const TimedDoor& door;

 public:
  explicit DoorTimerAdapter(const TimedDoor& door);
  void Timeout() override;
};

class TimedDoor : public Door {
 private:
  DoorTimerAdapter* adapter;
  Timer timer;
  int iTimeout;
  bool isOpened;

 public:
  explicit TimedDoor(int timeout);
  ~TimedDoor();
  bool isDoorOpened() const override;
  void unlock() override;
  void lock() override;
  int getTimeOut() const;
  void throwState() const;
};

#endif  // INCLUDE_TIMEDDOOR_H_
