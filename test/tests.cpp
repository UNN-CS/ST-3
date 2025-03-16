// Copyright 2021 GHA Test Team
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstdint>
#include <stdexcept>
#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door {
 public:
  MOCK_METHOD(void, lock, (), (override));
  MOCK_METHOD(void, unlock, (), (override));
  MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
  TimedDoor *door;
  Timer timer;

  void SetUp() override { door = new TimedDoor(1000); }

  void TearDown() override { delete door; }
};

TEST_F(TimedDoorTest, DoorInitiallyClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockDoor) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockDoor) {
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutThrowsExceptionWhenDoorLeftOpen) {
  door->unlock();
  EXPECT_THROW(timer.tregister(door->getTimeOut(), door->getAdapter()),
               std::runtime_error);
}

TEST_F(TimedDoorTest, NoExceptionWhenDoorClosedBeforeTimeout) {
  door->unlock();
  door->lock();
  EXPECT_NO_THROW(timer.tregister(door->getTimeOut(), door->getAdapter()));
}

TEST_F(TimedDoorTest, DoorRemainsLockedAfterLock) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockAndLockMultipleTimes) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST(TimerTest, TimerCallsTimeout) {
  MockTimerClient mockClient;
  Timer timer;

  EXPECT_CALL(mockClient, Timeout()).Times(1);
  timer.tregister(100, &mockClient);
}

TEST(TimerTest, TimerDoesNotCallTimeoutBeforeTimeout) {
  MockTimerClient mockClient;
  Timer timer;

  EXPECT_CALL(mockClient, Timeout()).Times(0);
}

TEST(DoorTimerAdapterTest, AdapterCallsThrowState) {
  MockDoor mockDoor;
  EXPECT_CALL(mockDoor, isDoorOpened())
      .Times(1)
      .WillOnce(::testing::Return(true));

  DoorTimerAdapter adapter(mockDoor);

  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, AdapterDoesNotThrowWhenDoorClosed) {
  MockDoor mockDoor;
  EXPECT_CALL(mockDoor, isDoorOpened())
      .Times(1)
      .WillOnce(::testing::Return(false));

  DoorTimerAdapter adapter(mockDoor);
  EXPECT_NO_THROW(adapter.Timeout());
}
