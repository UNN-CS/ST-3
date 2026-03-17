// Copyright 2021 GHA Test Team

#ifndef INCLUDE_TIMEDDOOR_H_
#define INCLUDE_TIMEDDOOR_H_


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

 public:
  explicit TimedDoor(int timeout);
  virtual ~TimedDoor() { delete adapter; }
  bool isDoorOpened() override;
  void unlock() override;
  void lock() override;
  int getTimeOut() const;
  void throwState();
};

class Timer {
 private:
  TimerClient* client;
  void sleep(int ms);

 public:
  void tregister(int timeout, TimerClient* client);
};

#endif  // INCLUDE_TIMEDDOOR_H_
