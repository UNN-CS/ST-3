// Copyright 2021 GHA Test Team
#ifndef INCLUDE_TIMEDDOOR_H_
#define INCLUDE_TIMEDDOOR_H_

#include <memory>
#include <mutex>
#include <thread>
#include <vector>

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

class TimedDoor;

class DoorTimerAdapter : public TimerClient {
 public:
  explicit DoorTimerAdapter(Door& d);
  void Timeout() override;
 private:
  Door& door;
};

class TimedDoor : public Door {
  friend class DoorTimerAdapter;
 public:
  explicit TimedDoor(int timeoutValue);
  ~TimedDoor() override;

  bool isDoorOpened() override;
  void unlock() override;
  void lock() override;
  int getTimeOut() const;
  void throwState();

  void triggerTimeout() {
    std::lock_guard<std::mutex> lock(mtx);
    isThrow = true;
  }

 private:
  std::mutex mtx;
  std::unique_ptr<DoorTimerAdapter> adapter;
  int iTimeout;
  bool isOpened;
  bool isThrow;
  std::vector<std::thread> threads;
};

class Timer {
  TimerClient* client;
  void sleep(int timeoutValue);
 public:
  void tregister(int timeoutValue, TimerClient* client);
};

#endif  // INCLUDE_TIMEDDOOR_H_
