// Copyright 2024 Nazyrov A.A.

#ifndef INCLUDE_TIMEDDOOR_H_
#define INCLUDE_TIMEDDOOR_H_

// Timer client interface
class TimerClient {
 public:
  virtual ~TimerClient() = default;
  virtual void Timeout() = 0;
};

// Door interface
class Door {
 public:
  virtual ~Door() = default;
  virtual void Lock() = 0;
  virtual void Unlock() = 0;
  virtual bool IsDoorOpened() = 0;
};

// Forward declaration
class TimedDoor;

// Adapter class
class DoorTimerAdapter : public TimerClient {
 private:
  TimedDoor& door_;

 public:
  explicit DoorTimerAdapter(TimedDoor& door);
  void Timeout() override;
};

// Timed door class
class TimedDoor : public Door {
 private:
  DoorTimerAdapter* adapter_;
  int timeout_seconds_;
  bool is_opened_;

 public:
  explicit TimedDoor(int timeout);
  ~TimedDoor();

  bool IsDoorOpened() override;
  void Unlock() override;
  void Lock() override;
  int GetTimeOut() const;
  void ThrowState();
};

// Timer class
class Timer {
 private:
  TimerClient* client_;
  void Sleep(int seconds);

 public:
  Timer();
  void TRegister(int timeout, TimerClient* client);
};

#endif  // INCLUDE_TIMEDDOOR_H_
