// Copyright 2025 Mikhail Burykin

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
 private:
  TimedDoor& door;
 public:
  explicit DoorTimerAdapter(TimedDoor&);
  ~DoorTimerAdapter() override = default;
  void Timeout() override;
};

class TimedDoor : public Door {
 private:
  DoorTimerAdapter * adapter;
  int iTimeout;
  bool isOpened;
 public:
  explicit TimedDoor(int);
  ~TimedDoor() override;
  bool isDoorOpened() override;
  void unlock() override;
  void lock() override;
  [[nodiscard]] int getTimeOut() const;
  static void throwState();
  void setAdapter(DoorTimerAdapter* newAdapter) {
   delete adapter;
   adapter = newAdapter;
  }
};

class Timer {
  TimerClient *client;
  static void sleep(int);
 public:
  Timer() : client(nullptr) {}
  void tregister(int, TimerClient*);
};

#endif  // INCLUDE_TIMEDDOOR_H_
