// Copyright 2021 GHA Test Team

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>

#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    door = std::make_unique<TimedDoor>(timeout);
  }

  void TearDown() override {
    door.reset();
  }

  const int timeout = 5;
  std::unique_ptr<TimedDoor> door;
};

class TimerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    client = std::make_unique<MockTimerClient>();
  }

  void TearDown() override {
    client.reset();
  }

  Timer timer;
  std::unique_ptr<MockTimerClient> client;
};

TEST_F(TimedDoorTest, DoorStartsClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutIsStored) {
  EXPECT_EQ(door->getTimeOut(), timeout);
}

TEST_F(TimedDoorTest, LockKeepsDoorClosed) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
  EXPECT_NO_THROW(door->unlock());
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockThenLockClosesDoor) {
  EXPECT_NO_THROW(door->unlock());
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, ThrowStateOnOpenDoorThrows) {
  EXPECT_NO_THROW(door->unlock());
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, ThrowStateOnClosedDoorDoesNotThrow) {
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, LockAfterUnlockAllowsSafeCheck) {
  EXPECT_NO_THROW(door->unlock());
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, SecondUnlockWorksAgain) {
  EXPECT_NO_THROW(door->unlock());
  door->lock();
  EXPECT_NO_THROW(door->unlock());
}

TEST_F(TimerTest, RegisterCallsClientTimeout) {
  EXPECT_CALL(*client, Timeout()).Times(1);
  timer.tregister(3, client.get());
}

TEST_F(TimerTest, RegisterCalledTwiceCallsClientTwice) {
  EXPECT_CALL(*client, Timeout()).Times(2);
  timer.tregister(1, client.get());
  timer.tregister(1, client.get());
}

TEST_F(TimerTest, RegisterWithNullClientDoesNotThrow) {
  EXPECT_NO_THROW(timer.tregister(1, nullptr));
}
