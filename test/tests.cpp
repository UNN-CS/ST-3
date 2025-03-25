// test/tests.cpp
// Copyright 2021 GHA Test Team
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <thread>
#include <memory>
#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorFixture : public ::testing::Test {
 protected:
  TimedDoor door{5};
  Timer timer;
  std::unique_ptr<MockTimerClient> mockClient;
  void SetUp() override {
    mockClient = std::make_unique<MockTimerClient>();
  }
};

TEST_F(TimedDoorFixture, DoorStartsClosed) {
  EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorFixture, UnlockOpensDoor) {
  door.unlock();
  EXPECT_TRUE(door.isDoorOpened());
}

TEST_F(TimedDoorFixture, LockClosesDoor) {
  door.unlock();
  door.lock();
  EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorFixture, LockAlreadyLockedThrows) {
  EXPECT_THROW(door.lock(), std::logic_error);
}

TEST_F(TimedDoorFixture, UnlockAlreadyUnlockedThrows) {
  door.unlock();
  EXPECT_THROW(door.unlock(), std::logic_error);
}

TEST_F(TimedDoorFixture, ThrowStateAlwaysThrows) {
  door.unlock();
  EXPECT_THROW(door.throwState(), std::runtime_error);
  door.lock();
  EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorFixture, TimerNullClientThrows) {
  EXPECT_THROW(timer.tregister(5, nullptr), std::invalid_argument);
}

TEST_F(TimedDoorFixture, TimerInvalidTimeoutThrows) {
  EXPECT_THROW(timer.tregister(0, mockClient.get()), std::invalid_argument);
  EXPECT_THROW(timer.tregister(-1, mockClient.get()), std::invalid_argument);
}

TEST_F(TimedDoorFixture, TimerTriggersTimeout) {
  EXPECT_CALL(*mockClient, Timeout()).Times(1);
  timer.tregister(1, mockClient.get());
}

TEST_F(TimedDoorFixture, DoorRemainsClosedIfLockedBeforeTimeout) {
  door.unlock();
  timer.tregister(3, mockClient.get());
  std::this_thread::sleep_for(std::chrono::seconds(1));
  door.lock();
  EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorFixture, DoorThrowsAfterTimeoutIfNotLocked) {
  door.unlock();
  timer.tregister(2, mockClient.get());
  std::this_thread::sleep_for(std::chrono::seconds(3));
  EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorFixture, MultipleLocksAndUnlocks) {
  door.unlock();
  EXPECT_TRUE(door.isDoorOpened());
  door.lock();
  EXPECT_FALSE(door.isDoorOpened());
  door.unlock();
  EXPECT_TRUE(door.isDoorOpened());
  door.lock();
  EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorFixture, MultipleUnlockLockSequence) {
  for (int i = 0; i < 3; i++) {
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
  }
}

TEST_F(TimedDoorFixture, TimerTriggersAfterExactTimeout) {
  EXPECT_CALL(*mockClient, Timeout()).Times(1);
  timer.tregister(2, mockClient.get());
  std::this_thread::sleep_for(std::chrono::seconds(2));
}
