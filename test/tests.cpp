// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdexcept>
#include "TimedDoor.h"

using ::testing::StrictMock;

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class DoorSystemTest : public ::testing::Test {
 protected:
  TimedDoor door{3};
};

TEST_F(DoorSystemTest, DoorStartsClosed) {
  EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(DoorSystemTest, UnlockOpensDoor) {
  door.unlock();
  EXPECT_TRUE(door.isDoorOpened());
}

TEST_F(DoorSystemTest, LockClosesDoor) {
  door.unlock();
  door.lock();
  EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(DoorSystemTest, GetTimeoutReturnsValue) {
  EXPECT_EQ(door.getTimeOut(), 3);
}

TEST_F(DoorSystemTest, ThrowStateDoesNotThrowWhenClosed) {
  EXPECT_NO_THROW(door.throwState());
}

TEST_F(DoorSystemTest, ThrowStateThrowsWhenOpen) {
  door.unlock();
  EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, TimeoutThrowsIfDoorOpen) {
  TimedDoor door(1);
  DoorTimerAdapter adapter(door);
  door.unlock();
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, TimeoutNoThrowIfDoorClosed) {
  TimedDoor door(1);
  DoorTimerAdapter adapter(door);
  EXPECT_NO_THROW(adapter.Timeout());
}

TEST(TimerTest, RegisterCallsTimeoutOnce) {
  Timer timer;
  StrictMock<MockTimerClient> client;
  EXPECT_CALL(client, Timeout()).Times(1);
  timer.tregister(0, &client);
}

TEST(TimerTest, RegisterWithNullClientDoesNotCrash) {
  Timer timer;
  EXPECT_NO_THROW(timer.tregister(0, nullptr));
}

TEST(TimerTest, RegisterAllowsNegativeTimeout) {
  Timer timer;
  StrictMock<MockTimerClient> client;
  EXPECT_CALL(client, Timeout()).Times(1);
  timer.tregister(-5, &client);
}

TEST(TimedDoorTest, MultipleUnlockLockKeepsState) {
  TimedDoor door(2);
  door.unlock();
  door.unlock();
  EXPECT_TRUE(door.isDoorOpened());
  door.lock();
  door.lock();
  EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, TimeoutValueIndependentOfState) {
  TimedDoor door(7);
  door.unlock();
  EXPECT_EQ(door.getTimeOut(), 7);
  door.lock();
  EXPECT_EQ(door.getTimeOut(), 7);
}

TEST(TimerAdapterIntegrationTest, TimeoutReflectsCurrentDoorState) {
  TimedDoor door(1);
  DoorTimerAdapter adapter(door);
  door.unlock();
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
  door.lock();
  EXPECT_NO_THROW(adapter.Timeout());
}
