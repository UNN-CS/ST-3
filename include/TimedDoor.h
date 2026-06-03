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
 public:
  explicit DoorTimerAdapter(TimedDoor& door);
  void Timeout() override;

 private:
  TimedDoor& door_;
};

class TimedDoor : public Door {
 public:
  explicit TimedDoor(int timeout);
  ~TimedDoor() override;

  bool isDoorOpened() override;
  void unlock() override;
  void lock() override;
  int getTimeOut() const;
  void throwState();

 private:
  DoorTimerAdapter* adapter_;
  int timeout_;
  bool opened_;
};

class Timer {
 public:
  void tregister(int timeout, TimerClient* client);

 private:
  void sleep(int milliseconds);
  TimerClient* client_ = nullptr;
};

#endif  // INCLUDE_TIMEDDOOR_H_
