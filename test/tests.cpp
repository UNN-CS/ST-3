// test/tests.cpp
// Copyright 2021 GHA Test Team
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"
#include <thread>
#include <chrono>
#include <stdexcept>

using std::chrono::milliseconds;
using std::chrono::seconds;

class TimedDoorTest : public ::testing::Test {
 protected:
  TimedDoor* door;
  void SetUp() override {
    door = new TimedDoor(1);
  }
  void TearDown() override {
    delete door;
  }
};

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

TEST_F(TimedDoorTest, DoorInitialStateIsClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockSetsDoorToOpen) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  std::this_thread::sleep_for(milliseconds(500));
  door->lock();
  std::this_thread::sleep_for(milliseconds(600));
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockSetsDoorToClosed) {
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
  std::this_thread::sleep_for(seconds(1));
}

TEST_F(TimedDoorTest, GetTimeOutReturnsCorrectValue) {
  EXPECT_EQ(door->getTimeOut(), 1);
}

TEST_F(TimedDoorTest, ExceptionThrownWhenDoorRemainsOpenAfterTimeout) {
  door->unlock();
  std::this_thread::sleep_for(seconds(2));
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, NoExceptionWhenDoorClosedBeforeTimeout) {
  door->unlock();
  std::this_thread::sleep_for(milliseconds(500));
  door->lock();
  std::this_thread::sleep_for(seconds(1));
  EXPECT_NO_THROW(door->throwState());
}

TEST(TimerAdapterTest, TimeoutCallsThrowStateIfDoorOpen) {
  TimedDoor door(0);
  door.unlock();
  EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST(TimerTest, TimerRegistersAndCallsTimeout) {
  auto mockClient = new MockTimerClient();
  Timer timer;
  EXPECT_CALL(*mockClient, Timeout()).Times(1);
  timer.tregister(1, mockClient);
  std::this_thread::sleep_for(seconds(2));
  delete mockClient;
}

TEST(TimerTest, TimerWithZeroTimeoutCallsImmediately) {
  auto mockClient = new MockTimerClient();
  Timer timer;
  EXPECT_CALL(*mockClient, Timeout()).Times(1);
  timer.tregister(0, mockClient);
  std::this_thread::sleep_for(milliseconds(100));
  delete mockClient;
}

TEST_F(TimedDoorTest, DoorThrowStateThrowsRuntimeError) {
  EXPECT_THROW(door->throwState(), std::runtime_error);
}
