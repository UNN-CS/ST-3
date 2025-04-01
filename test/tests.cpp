// Copyright 2021 GHA Test Team

#include "TimedDoor.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Mock;

class MockTimer : public Timer {
 public:
  MOCK_METHOD(void, tregister, (int, TimerClient*), (override));
  MOCK_METHOD(void, trigger, (), (override));
};

class MockTimedDoor : public TimedDoor {
 public:
  explicit MockTimedDoor(int timeout) : TimedDoor(timeout) {}
  MOCK_METHOD(void, throwState, (), (override));
};

TEST(TimedDoorTest, InitialStateIsClosed) {
  TimedDoor door(5);
  EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, UnlockOpensDoor) {
  TimedDoor door(5);
  door.unlock();
  EXPECT_TRUE(door.isDoorOpened());
}

TEST(TimedDoorTest, LockClosesDoor) {
  TimedDoor door(5);
  door.unlock();
  door.lock();
  EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, TimeoutThrowsWhenDoorIsOpen) {
  TimedDoor door(1);
  door.unlock();
  EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST(TimedDoorTest, NoThrowWhenDoorIsClosed) {
  TimedDoor door(1);
  EXPECT_NO_THROW(door.throwState());
}

TEST(DoorTimerAdapterTest, TimeoutCallsThrowStateWhenOpen) {
  MockTimedDoor door(0);
  door.unlock();
  DoorTimerAdapter adapter(door);
  EXPECT_CALL(door, throwState()).Times(1);
  adapter.Timeout();
}

TEST(DoorTimerAdapterTest, TimeoutNoCallWhenClosed) {
  MockTimedDoor door(0);
  door.lock();
  DoorTimerAdapter adapter(door);
  EXPECT_CALL(door, throwState()).Times(0);
  adapter.Timeout();
}

TEST(TimedDoorTest, GetTimeoutValue) {
  TimedDoor door(10);
  EXPECT_EQ(door.getTimeOut(), 10);
}

TEST(TimedDoorTest, CloseBeforeTimeout) {
  TimedDoor door(5);
  door.unlock();
  door.lock();
  EXPECT_NO_THROW(door.throwState());
}

TEST(TimedDoorTest, ZeroTimeoutImmediateTrigger) {
  TimedDoor door(0);
  door.unlock();
  EXPECT_THROW(door.getTimer()->trigger(), std::runtime_error);
}

TEST(TimerTest, TriggerWithoutRegistration) {
  RealTimer timer;
  EXPECT_NO_THROW(timer.trigger());
}

TEST(TimedDoorTest, LockWhileTimerPending) {
  TimedDoor door(10);
  door.unlock();
  door.lock();
  EXPECT_NO_THROW(door.getTimer()->trigger());
}

TEST(TimedDoorTest, StateAfterException) {
  TimedDoor door(1);
  door.unlock();
  try { door.throwState(); } catch (...) {}
  EXPECT_TRUE(door.isDoorOpened());
}

TEST(TimedDoorTest, StressTest) {
  for (int i = 0; i < 1000; ++i) {
    TimedDoor door(1);
    door.unlock();
    door.lock();
  }
}

TEST(DoorTimerAdapterTest, MultipleTriggers) {
  MockTimedDoor door(0);
  door.unlock();
  DoorTimerAdapter adapter(door);
  EXPECT_CALL(door, throwState()).Times(3);
  adapter.Timeout();
  adapter.Timeout();
  adapter.Timeout();
}
