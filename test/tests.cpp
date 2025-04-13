// Copyright 2023 Your Name
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <thread>
#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
  TimedDoor* door;
  MockTimerClient* mockClient;

  void SetUp() override {
    door = new TimedDoor(50);
    mockClient = new MockTimerClient();
  }

  void TearDown() override {
    delete door;
    delete mockClient;
  }
};

TEST_F(TimedDoorTest, InitialState) { EXPECT_FALSE(door->isDoorOpened()); }

TEST_F(TimedDoorTest, OpenDoor) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, CloseDoor) {
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, ExceptionOnTimeout) {
  door->unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(150));
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, NoExceptionWhileClosed) {
  door->unlock();
  door->lock();
  std::this_thread::sleep_for(std::chrono::milliseconds(150));
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, TimerResetOnReopen) {
  door->unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(3));
  door->lock();
  door->unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(3));
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, AdapterTriggerTimeout) {
  DoorTimerAdapter adapter(*door);
  door->unlock();
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, GetTimeoutVal) { EXPECT_EQ(door->getTimeOut(), 50); }

TEST_F(TimedDoorTest, ReopenAfterClose) {
  door->unlock();
  door->lock();
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, CloseAgain) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}
