// Copyright 2021 GHA Test Team

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>

#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
  TimedDoor* door;
  DoorTimerAdapter* adapter;

  void SetUp() override {
    door = new TimedDoor(1);
    adapter = new DoorTimerAdapter(*door);
  }

  void TearDown() override {
    delete adapter;
    delete door;
  }
};

TEST_F(TimedDoorTest, InitialStateIsClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeOutReturnsCorrectValue) {
  EXPECT_EQ(door->getTimeOut(), 1);
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

TEST_F(TimedDoorTest, ThrowStateDoesNotThrowWhenClosed) {
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, ThrowStateThrowsWhenOpen) {
  door->unlock();
  EXPECT_THROW(door->throwState(), int);
}

TEST_F(TimedDoorTest, AdapterTimeoutThrowsWhenDoorOpen) {
  door->unlock();
  EXPECT_THROW(adapter->Timeout(), int);
}

TEST_F(TimedDoorTest, AdapterTimeoutNoThrowWhenDoorClosed) {
  EXPECT_NO_THROW(adapter->Timeout());
}

TEST(TimerTest, TimerRegistersAndCallsTimeout) {
  Timer timer;
  MockTimerClient mockClient;

  EXPECT_CALL(mockClient, Timeout()).Times(1);

  timer.tregister(0, &mockClient);
}

TEST(TimerTest, FullIntegrationThrowsWhenDoorOpen) {
  TimedDoor door(0);
  DoorTimerAdapter adapter(door);
  Timer timer;

  door.unlock();

  EXPECT_THROW(timer.tregister(0, &adapter), int);
}

TEST(TimerTest, FullIntegrationNoThrowWhenDoorClosed) {
  TimedDoor door(0);
  DoorTimerAdapter adapter(door);
  Timer timer;

  door.lock();

  EXPECT_NO_THROW(timer.tregister(0, &adapter));
}
