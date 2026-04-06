// Copyright 2026 Yakimov
#include "../include/TimedDoor.h"

#include <chrono>
#include <stdexcept>
#include <thread>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Return;

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
  void SetUp() override {
    door = new TimedDoor(100);
  }

  void TearDown() override {
    delete door;
  }

  TimedDoor* door;
};

class DoorTimerAdapterTest : public ::testing::Test {
 protected:
  void SetUp() override {
    door = new TimedDoor(100);
    adapter = new DoorTimerAdapter(*door);
  }

  void TearDown() override {
    delete adapter;
    delete door;
  }

  TimedDoor* door;
  DoorTimerAdapter* adapter;
};

TEST_F(TimedDoorTest, ConstructorInitializesCorrectly) {
  EXPECT_FALSE(door->isDoorOpened());
  EXPECT_EQ(door->getTimeOut(), 100);
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutThrowsExceptionWhenDoorIsOpen) {
  door->unlock();
  DoorTimerAdapter adapter(*door);
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, TimeoutDoesNotThrowExceptionWhenDoorIsClosed) {
  door->lock();
  DoorTimerAdapter adapter(*door);
  EXPECT_NO_THROW(adapter.Timeout());
}

TEST_F(DoorTimerAdapterTest, AdapterCallsThrowStateWhenDoorOpen) {
  door->unlock();
  EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

TEST_F(DoorTimerAdapterTest, AdapterDoesNotThrowWhenDoorClosed) {
  door->lock();
  EXPECT_NO_THROW(adapter->Timeout());
}

TEST(TimerTest, TimerCallsTimeoutOnClient) {
  MockTimerClient mockClient;
  Timer timer;

  EXPECT_CALL(mockClient, Timeout()).Times(1);

  timer.tregister(10, &mockClient);
}

TEST(TimerTest, TimerDoesNotCallWithNullClient) {
  Timer timer;
  timer.tregister(10, nullptr);
  SUCCEED();
}

TEST_F(TimedDoorTest, MultipleOpenCloseCycles) {
  for (int i = 0; i < 3; ++i) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
  }
}

TEST_F(TimedDoorTest, GetTimeOutReturnsCorrectValue) {
  EXPECT_EQ(door->getTimeOut(), 100);
}

TEST(TimerIntegrationTest, TimerTriggersAfterTimeout) {
  TimedDoor door(50);
  door.unlock();
  Timer timer;

  EXPECT_THROW({
    timer.tregister(50, door.getAdapter());
  }, std::runtime_error);
}
