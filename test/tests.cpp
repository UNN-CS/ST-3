// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
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
  TimedDoor* door;

  void SetUp() override {
    door = new TimedDoor(1);
  }

  void TearDown() override {
    delete door;
  }
};


TEST_F(TimedDoorTest, DoorInitiallyClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
  try {
    door->unlock();
  } catch (...) {
  }
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockThrowsIfStillOpen) {
  EXPECT_THROW(door->unlock(), std::runtime_error);
}

TEST_F(TimedDoorTest, ThrowStateAlwaysThrows) {
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST(TimerTest, TimerWorksWithoutError) {
  Timer t;
  MockTimerClient client;

  EXPECT_NO_THROW(t.tregister(0, &client));
}

TEST(AdapterTest, AdapterThrowsException) {
  TimedDoor door(0);

  EXPECT_THROW(door.unlock(), std::runtime_error);
}

TEST(MockTest, TimerClientMethodCalled) {
  MockTimerClient mock;

  EXPECT_CALL(mock, Timeout()).Times(1);
  mock.Timeout();
}

TEST(MockDoorTest, DoorMethodsCalled) {
  MockDoor door;

  EXPECT_CALL(door, lock()).Times(1);
  EXPECT_CALL(door, unlock()).Times(1);

  door.lock();
  door.unlock();
}

TEST(MockDoorTest, DoorReturnsState) {
  MockDoor door;

  EXPECT_CALL(door, isDoorOpened())
      .WillOnce(::testing::Return(true));

  EXPECT_TRUE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, LockUnlockSequence) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());

  try {
    door->unlock();
  } catch (...) {
  }

  EXPECT_TRUE(door->isDoorOpened());
}
