// Copyright 2021 GHA Test Team

class TimedDoor : public Door {
 private:
  DoorTimerAdapter* adapter;
  Timer* timer;
  int iTimeout;
  bool isOpened;
 public:
  explicit TimedDoor(int);
  bool isDoorOpened() override;
  void unlock() override;
  void lock() override;
  int getTimeOut() const;
  void throwState();
  Timer* getTimer();
};

class Timer {
  TimerClient* client;
 public:
  void tregister(int, TimerClient*);
  void trigger();
};
