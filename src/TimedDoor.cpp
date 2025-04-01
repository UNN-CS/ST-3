// Copyright 2021 GHA Test Team

#include "TimedDoor.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>


using ::testing::_;
using ::testing::AtLeast;
using ::testing::Throw;

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

TEST(TimedDoorTest, DoubleUnlockThrowsTwice) {
  TimedDoor door(0);
  door.unlock();
  EXPECT_THROW(door.throwState(), std::runtime_error);
  door.lock();
  door.unlock();
  EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST(TimedDoorTest, CloseBeforeTimeout) {
  TimedDoor door(5);
  door.unlock();
  door.lock();
  EXPECT_NO_THROW(door.throwState());
}
