// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <stdexcept>
#include <thread>

#include "TimedDoor.h"

class MockDoorTimerAdapter : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
  TimedDoor* door;
  Timer* timer;
  MockDoorTimerAdapter* mockAdapter;

  void SetUp() override {
    door = new TimedDoor(5);
    timer = new Timer();
    mockAdapter = new MockDoorTimerAdapter();
  }

  void TearDown() override {
    delete door;
    delete timer;
    delete mockAdapter;
  }
};

TEST_F(TimedDoorTest, DoorIsInitiallyClosed) {
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

TEST_F(TimedDoorTest, ImpossibleToLockAlreadyClosedDoor) {
  EXPECT_THROW(door->lock(), std::runtime_error);
}

TEST_F(TimedDoorTest, ImpossibleToUnlockAlreadyOpenDoor) {
  door->unlock();
  EXPECT_THROW(door->unlock(), std::runtime_error);
}

TEST_F(TimedDoorTest, DoorThrowsState) {
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, GetTimeOutReturnsCorrectValue) {
  EXPECT_EQ(door->getTimeOut(), 5);
}

TEST_F(TimedDoorTest, TimeoutCalledWhenDoorIsOpen) {
  EXPECT_CALL(*mockAdapter, Timeout()).Times(1);
  door->unlock();
}

TEST_F(TimedDoorTest, TimeoutCalledWhenTimerIsTregister) {
  EXPECT_CALL(*mockAdapter, Timeout()).Times(1);
  timer->tregister(door->getTimeOut(), mockAdapter);
}

TEST_F(TimedDoorTest, TimerThrowsOnNegativeTimeout) {
  EXPECT_THROW(timer->tregister(-1, mockAdapter), std::runtime_error);
}

TEST_F(TimedDoorTest, TimerThrowsOnNullClient) {
  EXPECT_THROW(timer->tregister(5, nullptr), std::runtime_error);
}
