// Copyright 2025 Zinoviev Alexander

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"

using ::testing::_;
using ::testing::Return;

class MockTimedDoor : public TimedDoor {
 public:
  explicit MockTimedDoor(int timeout) : TimedDoor(timeout) {}
  MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

TEST(DoorTimerAdapterTest, TimeoutCallsThrowStateWhenDoorIsOpen) {
  MockTimedDoor door(1000);
  EXPECT_CALL(door, isDoorOpened()).WillOnce(Return(true));
  DoorTimerAdapter adapter(door);
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, TimeoutDoesNotCallThrowStateWhenDoorIsClosed) {
  MockTimedDoor door(1000);
  EXPECT_CALL(door, isDoorOpened()).WillOnce(Return(false));
  DoorTimerAdapter adapter(door);
  EXPECT_NO_THROW(adapter.Timeout());
}

TEST(TimedDoorTest, IsInitiallyClosed) {
  TimedDoor door(1000);
  EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, UnlockOpensDoor) {
  TimedDoor door(0);
  door.unlock();
  EXPECT_TRUE(door.isDoorOpened());
}

TEST(TimedDoorTest, LockClosesDoor) {
  TimedDoor door(0);
  door.unlock();
  door.lock();
  EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, TimerRegistrationWhenUnlocked) {
  TimedDoor door(100);
  EXPECT_NO_THROW(door.unlock());
  EXPECT_TRUE(door.isDoorOpened());
}

TEST(TimedDoorTest, GetTimeoutReturnsCorrectValue) {
  TimedDoor door(500);
  EXPECT_EQ(door.getTimeOut(), 500);
}

TEST(TimedDoorTest, MultipleUnlockLockCycles) {
  TimedDoor door(100);
  for (int i = 0; i < 3; ++i) {
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
  }
}

TEST(TimerTest, CallsTimeoutAfterTime) {
  MockTimerClient client;
  EXPECT_CALL(client, Timeout()).Times(1);
  Timer timer;
  timer.tregister(0, &client);
}

TEST(TimedDoorTest, ZeroTimeoutDoesNotRegisterTimer) {
  TimedDoor door(0);
  door.unlock();
  EXPECT_NO_THROW(std::this_thread::sleep_for(std::chrono::milliseconds(10)));
}
