// Copyright 2025 UNN-CS
#ifndef INCLUDE_TIMEDDOOR_H_
#define INCLUDE_TIMEDDOOR_H_

#include <stdexcept>

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

class DoorTimeoutException : public std::runtime_error {
 public:
  explicit DoorTimeoutException(const char* message)
      : std::runtime_error(message) {}
};

class TimedDoor;

class DoorTimerAdapter : public TimerClient {
 private:
  TimedDoor& door;

 public:
  explicit DoorTimerAdapter(TimedDoor&);
  void Timeout() override;
};

class Timer {
 private:
  TimerClient* client;
  void sleep(int seconds);

 public:
  Timer();
  void tregister(int seconds, TimerClient* timerClient);
};

class TimedDoor : public Door {
 private:
  DoorTimerAdapter* adapter;
  Timer* timer;
  int iTimeout;
  bool isOpened;

 public:
  explicit TimedDoor(int timeout);
  ~TimedDoor() override;

  bool isDoorOpened() override;
  void unlock() override;
  void lock() override;
  int getTimeOut() const;
  virtual void throwState();

  Timer* getTimer() const;
  void triggerTimeoutForTest();
  void registerTimerForTest(int timeout, TimerClient* client);
};

#endif  // INCLUDE_TIMEDDOOR_H_
