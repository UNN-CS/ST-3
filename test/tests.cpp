// Copyright 2026 Maxim Safronov
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstdint>
#include <stdexcept>
#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class DoorTest : public ::testing::Test {
 protected:
  TimedDoor *door;

  void SetUp() override { door = new TimedDoor(1); }
  void TearDown() override { delete door; }
};

TEST_F(DoorTest, ClosedByDefault) { EXPECT_FALSE(door->isDoorOpened()); }

TEST_F(DoorTest, UnlockOpens) {
  TimedDoor fastDoor(0);
  EXPECT_THROW(fastDoor.unlock(), std::runtime_error);
  EXPECT_TRUE(fastDoor.isDoorOpened());
}

TEST_F(DoorTest, LockCloses) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(DoorTest, TimeoutValue) { EXPECT_EQ(door->getTimeOut(), 1); }

TEST_F(DoorTest, UnlockThrows) {
  EXPECT_THROW(door->unlock(), std::runtime_error);
}

TEST_F(DoorTest, ThrowStateClosed) {
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

TEST(AdapterTest, AdapterThrowsOpen) {
  TimedDoor door(0);
  DoorTimerAdapter adapter(door);
  EXPECT_THROW(door.unlock(), std::runtime_error);
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(AdapterTest, AdapterNoThrowClosed) {
  TimedDoor door(0);
  DoorTimerAdapter adapter(door);
  door.lock();
  EXPECT_NO_THROW(adapter.Timeout());
}

TEST(TimerTest, CallsTimeout) {
  Timer timer;
  MockTimerClient mock;
  EXPECT_CALL(mock, Timeout()).Times(1);
  timer.tregister(0, &mock);
}

TEST(TimerTest, NullClientSafe) {
  Timer timer;
  EXPECT_NO_THROW(timer.tregister(0, nullptr));
}

TEST_F(DoorTest, TimeoutThrows) {
  TimedDoor fastDoor(0);
  EXPECT_THROW(fastDoor.unlock(), std::runtime_error);
  EXPECT_THROW(fastDoor.throwState(), std::runtime_error);
}
