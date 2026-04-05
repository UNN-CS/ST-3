// Copyright 2021 GHA Test Team

#ifndef INCLUDE_TIMEDDOOR_H_
#define INCLUDE_TIMEDDOOR_H_

#include <atomic>
#include <memory>
#include <thread>

class DoorTimerAdapter;
class Timer;
class Door;
class TimedDoor;

class TimerClient {
 public:
  virtual ~TimerClient() {}
  virtual void Timeout() = 0;
};

class Door {
 public:
  virtual ~Door() {}
  virtual void lock() = 0;
  virtual void unlock() = 0;
  virtual bool isDoorOpened() = 0;
};

class DoorTimerAdapter : public TimerClient {
 private:
  TimedDoor* door_;
  std::shared_ptr<std::atomic<bool> > cancelled_;

 public:
  explicit DoorTimerAdapter(TimedDoor* d);
  void Timeout() override;
  void cancel();
  std::shared_ptr<std::atomic<bool> > getCancelled() const {
    return cancelled_;
  }
};

class TimedDoor : public Door {
 private:
  DoorTimerAdapter* adapter_;
  int iTimeout_;
  std::atomic<bool> isOpened_;
  std::thread timerThread_;
  std::shared_ptr<std::atomic<bool> > currentCancelled_;

 public:
  explicit TimedDoor(int timeout);
  ~TimedDoor();
  bool isDoorOpened() override;
  void unlock() override;
  void lock() override;
  int getTimeOut() const;
  void throwState();
};

class Timer {
 public:
  static void tregister(int timeout, TimerClient* client);
};

#endif  // INCLUDE_TIMEDDOOR_H_
