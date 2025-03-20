// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>

#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
  TimedDoor *door;

  void SetUp() override {
    door = new TimedDoor(1);
  }

  void TearDown() override {
    delete door;
  }
};

TEST_F(TimedDoorTest, DoorInitiallyClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, ThrowStateThrowsWhenDoorOpen) {
  door->unlock();
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, ThrowStateDoesNotThrowWhenDoorClosed) {
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, UnlockDoesNotChangeStateIfAlreadyOpened) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());

  door->unlock();

  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, RepeatedUnlockDoesNotThrowUntilTimeout) {
  door->unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  door->unlock();

  std::this_thread::sleep_for(std::chrono::seconds(2));
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, MultipleLockUnlockCycles) {
  for (int i = 0; i < 3; ++i) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
  }
}

TEST_F(TimedDoorTest, TimeoutDoesNotThrowAfterLockingWithinTime) {
  door->unlock();
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, GetTimeoutReturnsCorrectValue) {
  EXPECT_EQ(door->getTimeOut(), 1);
}
