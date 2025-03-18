// Copyright 2021 GHA Test Team

#ifndef INCLUDE_TIMEDDOOR_H_
#define INCLUDE_TIMEDDOOR_H_
#include <iostream>

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
	virtual bool isDoorOpened() const = 0;
};

class Timer {
	std::weak_ptr<TimerClient> m_client;
	void sleep(int);
public:
	void tregister(int time, std::weak_ptr<TimerClient> client);
};

class DoorTimerAdapter : public TimerClient, public std::enable_shared_from_this<DoorTimerAdapter> {
	std::weak_ptr<TimedDoor> m_door;
	Timer m_timer;
	int m_baseSleepTime = 0;
public:
	DoorTimerAdapter() = default;
	void SetTimedDoorData(int baseSleepTime, std::weak_ptr<TimedDoor> door);
	MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoor : public Door, public std::enable_shared_from_this<TimedDoor> {
	DoorTimerAdapter m_adapter;
	int m_timeout = 0;
	bool m_isOpened = false;
public:
	explicit TimedDoor(int time);
	MOCK_METHOD(bool, isDoorOpened, (), (const override));
	MOCK_METHOD(void, unlock, (), (override));
	MOCK_METHOD(void, lock, (), (override));
	MOCK_METHOD(int, getTimeOut, (), (const override));
	MOCK_METHOD(void, throwState, (), (override));
};


#endif  // INCLUDE_TIMEDDOOR_H_
