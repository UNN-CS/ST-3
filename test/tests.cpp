// Copyright 2026 NNTU-CS
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdexcept>
#include "TimedDoor.h"

using ::testing::AtLeast;

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

struct TimedDoorTest : public testing::Test {
  TimedDoor* door;
  void SetUp() {
    door = new TimedDoor(1);
  }
  void TearDown() {
    delete door;
  }
};

TEST_F(TimedDoorTest, DoorInitiallyLocked) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorOpensAfterUnlock) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorClosesAfterLock) {
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeoutReturnsCorrectValue) {
  EXPECT_EQ(1, door->getTimeOut());
}

TEST_F(TimedDoorTest, ThrowStateThrowsWhenOpen) {
  door->unlock();
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, ThrowStateNoThrowWhenClosed) {
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, ThrowStateNoThrowInitially) {
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, UnlockThenLockNoThrow) {
  door->unlock();
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, MultipleOpenClose) {
  for (int i = 0; i < 5; ++i) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
  }
}

TEST_F(TimedDoorTest, TimeoutOnOpenDoorThrows) {
  door->unlock();
  DoorTimerAdapter adapter(*door);
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, TimeoutOnClosedDoorNoThrow) {
  door->lock();
  DoorTimerAdapter adapter(*door);
  EXPECT_NO_THROW(adapter.Timeout());
}

TEST(MockTimerClientTest, TimeoutIsCalled) {
  MockTimerClient mockClient;
  EXPECT_CALL(mockClient, Timeout()).Times(AtLeast(1));
  mockClient.Timeout();
}

TEST(MockDoorTest, LockAndUnlockAreCalled) {
  MockDoor mockDoor;
  EXPECT_CALL(mockDoor, lock()).Times(1);
  EXPECT_CALL(mockDoor, unlock()).Times(1);
  mockDoor.unlock();
  mockDoor.lock();
}
