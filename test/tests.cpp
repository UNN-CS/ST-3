// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdexcept>
#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    timeout_ms_ = 1;
  }
  int timeout_ms_;
};

TEST_F(TimedDoorTest, DoorStartsLocked) {
  TimedDoor door(timeout_ms_);
  EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
  TimedDoor door(timeout_ms_);
  door.unlock();
  EXPECT_TRUE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
  TimedDoor door(timeout_ms_);
  door.unlock();
  door.lock();
  EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeoutReturnsConfiguredValue) {
  const int expected = 5000;
  TimedDoor door(expected);
  EXPECT_EQ(expected, door.getTimeOut());
}

TEST_F(TimedDoorTest, ThrowStateThrowsException) {
  TimedDoor door(timeout_ms_);
  EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, AdapterTimeoutThrowsWhenDoorOpen) {
  TimedDoor door(1);
  door.unlock();
  DoorTimerAdapter* adapter = door.getAdapter();
  EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, AdapterTimeoutNoThrowWhenDoorClosed) {
  TimedDoor door(1);
  door.lock();
  DoorTimerAdapter* adapter = door.getAdapter();
  EXPECT_NO_THROW(adapter->Timeout());
}

TEST(TimerTest, TimerCallsClientTimeout) {
  MockTimerClient mock_client;
  Timer timer;
  EXPECT_CALL(mock_client, Timeout()).Times(1);
  timer.tregister(1, &mock_client);
}

TEST(TimerTest, TimerHandlesNullClient) {
  Timer timer;
  EXPECT_NO_THROW(timer.tregister(1, nullptr));
}

TEST_F(TimedDoorTest, MultipleLockUnlockCycles) {
  TimedDoor door(timeout_ms_);
  for (int i = 0; i < 5; ++i) {
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
  }
}

TEST(TimedDoorIntegrationTest, TimerWithClosedDoorNoException) {
  TimedDoor door(1);
  door.lock();
  Timer timer;
  EXPECT_NO_THROW(timer.tregister(1, door.getAdapter()));
}

TEST(TimedDoorIntegrationTest, TimerWithOpenDoorThrows) {
  TimedDoor door(1);
  door.unlock();
  Timer timer;
  EXPECT_THROW(timer.tregister(1, door.getAdapter()), std::runtime_error);
}
