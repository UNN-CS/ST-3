// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"

using namespace testing;

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
  try {
    door->unlock();
  }
  catch (const std::runtime_error &) {
  }
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
  try {
    door->unlock();
  }
  catch (...) {
  }
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

TEST(TimerTest, TimerCallsTimeout) {
  Timer timer;
  MockTimerClient mockClient;
  EXPECT_CALL(mockClient, Timeout()).Times(1);
  timer.tregister(1, &mockClient);
}

TEST(TimerClientTest, AdapterTimeoutDelegatesToDoorThrowState) {
  TimedDoor door(1);
  EXPECT_THROW(door.unlock(), std::runtime_error);
}

TEST_F(TimedDoorTest, MultipleLockUnlockCycles) {
  for (int i = 0; i < 3; ++i) {
    try {
      door->unlock();
    }
    catch (...) {
    }
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
  }
}

TEST_F(TimedDoorTest, TimeoutDoesNotThrowAfterLockingWithinTime) {
  try {
    door->unlock();
  }
  catch (...) {
  }
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}
