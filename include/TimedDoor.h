// Copyright 2021 GHA Test Team

#ifndef INCLUDE_TIMEDDOOR_H_
#define INCLUDE_TIMEDDOOR_H_

#include <stdexcept>

class DoorTimerAdapter;
class Timer;
class Door;
class TimedDoor;

class DoorTimeoutException : public std::runtime_error {
 public:
  explicit DoorTimeoutException(const char *msg) : std::runtime_error(msg) {}
};

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
  TimedDoor &door;

 public:
  explicit DoorTimerAdapter(TimedDoor &);
  void Timeout() override;
};

class Timer {
 private:
  TimerClient *client;
  void sleep(int);

 public:
  Timer();
  void tregister(int, TimerClient *);
};

class TimedDoor : public Door {
 private:
  DoorTimerAdapter adapter;
  Timer timer;
  int iTimeout;
  bool isOpened;

 public:
  explicit TimedDoor(int);
  bool isDoorOpened() override;
  void unlock() override;
  void lock() override;
  int getTimeOut() const;
  virtual void throwState();
  Timer *getTimer();
  void triggerTimeoutForTest();
  void registerTimerForTest(int timeout, TimerClient *client);
};

#endif // INCLUDE_TIMEDDOOR_H_
