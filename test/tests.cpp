// Copyright 2026 GHA Test Team

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    door = new TimedDoor(1);
  }

  void TearDown() override {
    delete door;
  }

  TimedDoor *door;
};

TEST_F(TimedDoorTest, InitialStateIsClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeOutReturnsCorrectValue) {
  EXPECT_EQ(door->getTimeOut(), 1);
}

TEST_F(TimedDoorTest, LockAndUnlockChangeState) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, MultipleUnlocksDoNotCreateMultipleTimers) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, ThrowStateThrowsExceptionWhenDoorOpen) {
  door->unlock();
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, ThrowStateDoesNotThrowWhenDoorClosed) {
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, ExceptionMessageIsCorrect) {
  door->unlock();
  try {
    door->throwState();
    FAIL() << "Expected std::runtime_error";
  } catch (const std::runtime_error& e) {
    EXPECT_STREQ(e.what(), "Door timeout: door was left open too long!");
  }
}

TEST_F(TimedDoorTest, SequentialOpenCloseOperations) {
  for (int i = 0; i = 3; ++i) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
  }
}

class DoorTimerAdapterTest : public ::testing::Test {
 protected:
  void SetUp() override {
    door = new TimedDoor(1);
    adapter = new DoorTimerAdapter(*door);
  }

  void TearDown() override {
    delete adapter;
    delete door;
  }

  TimedDoor *door;
  DoorTimerAdapter *adapter;
};

TEST_F(DoorTimerAdapterTest, AdapterTimeoutThrowsWhenDoorOpen) {
  door->unlock();
  EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

TEST_F(DoorTimerAdapterTest, AdapterTimeoutNoThrowWhenDoorClosed) {
  door->lock();
  EXPECT_NO_THROW(adapter->Timeout());
}

TEST(TimerTest, TimerCallsTimeoutOnClient) {
  Timer timer;
  MockTimerClient mockClient;
  EXPECT_CALL(mockClient, Timeout()).Times(1);
  timer.tregister(0, &mockClient);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(TimedDoorIntegrationTest, DoorThrowsExceptionAfterTimeout) {
  TimedDoor door(1);
  door.unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(1500));
  EXPECT_TRUE(door.isDoorOpened());
  EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST(TimedDoorIntegrationTest, ClosingDoorBeforeTimeoutPreventsException) {
  TimedDoor door(1);
  door.unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  door.lock();
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  EXPECT_NO_THROW(door.throwState());
}
