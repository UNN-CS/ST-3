// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdexcept>
#include "TimedDoor.h"

using ::testing::InSequence;
using ::testing::Return;

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door {
 public:
  MOCK_METHOD(void, lock,         (), (override));
  MOCK_METHOD(void, unlock,       (), (override));
  MOCK_METHOD(bool, isDoorOpened, (), (override));
};

static void openThenCloseIfOpen(Door& door) {
  door.unlock();
  if (door.isDoorOpened())
    door.lock();
}

class TimedDoorTest : public ::testing::Test {
 protected:
  TimedDoor* instant;
  TimedDoor* delayed;

  void SetUp() override {
    instant = new TimedDoor(0);
    delayed = new TimedDoor(2);
  }
  void TearDown() override {
    delete instant;
    delete delayed;
  }
};

TEST_F(TimedDoorTest, newDoorIsClosed) {
  EXPECT_FALSE(instant->isDoorOpened());
}

TEST_F(TimedDoorTest, getTimeout) {
  EXPECT_EQ(0, instant->getTimeOut());
  EXPECT_EQ(2, delayed->getTimeOut());
}

TEST_F(TimedDoorTest, throwStateThrows) {
  EXPECT_THROW(instant->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, lockClosesDoor) {
  EXPECT_THROW(instant->unlock(), std::runtime_error);
  EXPECT_TRUE(instant->isDoorOpened());
  instant->lock();
  EXPECT_FALSE(instant->isDoorOpened());
}

TEST_F(TimedDoorTest, unlockZeroTimeoutThrows) {
  EXPECT_THROW(instant->unlock(), std::runtime_error);
}

TEST(AdapterTest, noThrowWhenClosed) {
  TimedDoor door(0);
  DoorTimerAdapter adapter(door);
  EXPECT_NO_THROW(adapter.Timeout());
}

TEST(AdapterTest, throwsWhenOpen) {
  TimedDoor door(0);
  DoorTimerAdapter adapter(door);
  EXPECT_THROW(door.unlock(), std::runtime_error);
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(TimerTest, callsTimeout) {
  Timer timer;
  MockTimerClient client;
  EXPECT_CALL(client, Timeout()).Times(1);
  EXPECT_NO_THROW(timer.tregister(0, &client));
}

TEST(TimerTest, nullClient) {
  Timer timer;
  EXPECT_NO_THROW(timer.tregister(0, nullptr));
}

TEST(DoorInterface, openAndClose) {
  MockDoor door;
  InSequence seq;
  EXPECT_CALL(door, unlock()).Times(1);
  EXPECT_CALL(door, isDoorOpened()).WillOnce(Return(true));
  EXPECT_CALL(door, lock()).Times(1);
  openThenCloseIfOpen(door);
}

TEST(DoorInterface, noLockIfClosed) {
  MockDoor door;
  EXPECT_CALL(door, unlock()).Times(1);
  EXPECT_CALL(door, isDoorOpened()).WillOnce(Return(false));
  EXPECT_CALL(door, lock()).Times(0);
  openThenCloseIfOpen(door);
}
