// Copyright 2021 GHA Test Team

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <thread>

#include "TimedDoor.h"

class FakeDoorTimerAdapter final : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class FakeTimedDoor final : public Door {
 public:
  MOCK_METHOD(void, lock, (), (override));
  MOCK_METHOD(void, unlock, (), (override));
  MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class CustomizableTimer {
 public:
  virtual ~CustomizableTimer() = default;

  virtual void tregister(int, TimerClient *) = 0;
};

class FakeTimer final : public CustomizableTimer {
 public:
  MOCK_METHOD(void, tregister, (int, TimerClient *), (override));
};

class TimedDoorTestSuite : public testing::Test {
 protected:
  void SetUp() override {
    door = new TimedDoor(3);
  }

  void TearDown() override {
    delete door;
  }

  TimedDoor *door = nullptr;
};

TEST_F(TimedDoorTestSuite, ThrowsOnOpenStateCheck) {
  door->unlock();
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTestSuite, StartsLocked) {
  EXPECT_FALSE(door->isDoorOpened());
  EXPECT_EQ(door->getTimeout(), 3);
}

TEST_F(TimedDoorTestSuite, CanBeLockedAgain) {
  door->lock();
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTestSuite, CanBeUnlocked) {
  door->lock();
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTestSuite, MultipleUnlocksAreSafe) {
  door->unlock();
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTestSuite, MultipleLocksAreSafe) {
  door->lock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTestSuite, ZeroTimeoutIsInvalid) {
  EXPECT_THROW(TimedDoor invalidTimeout(0), std::invalid_argument);
}

TEST_F(TimedDoorTestSuite, NegativeTimeoutIsInvalid) {
  EXPECT_THROW(TimedDoor negativeTimeout(-2), std::invalid_argument);
}

TEST_F(TimedDoorTestSuite, LockWhileClosedDoesNothing) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

class TimerClientTestSuite : public testing::Test {
 protected:
  void SetUp() override {
    door = new TimedDoor(4);
    timer_client = new DoorTimerAdapter(*door);
  }

  void TearDown() override {
    delete timer_client;
    delete door;
  }

  TimedDoor *door = nullptr;
  DoorTimerAdapter *timer_client = nullptr;
};

TEST_F(TimerClientTestSuite, ThrowsTimeoutWhenOpen) {
  door->lock();
  door->unlock();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  EXPECT_THROW(timer_client->Timeout(), std::runtime_error);
}

TEST_F(TimerClientTestSuite, NoThrowTimeoutWhenClosed) {
  door->lock();
  EXPECT_NO_THROW(timer_client->Timeout());
}
