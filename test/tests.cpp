// Copyright 2024 Nazyrov A.A.

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"
#include <chrono>
#include <thread>

using ::testing::_;
using ::testing::Mock;

// Mock TimerClient for testing
class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

// Mock Door for testing
class MockDoor : public Door {
 public:
  MOCK_METHOD(void, Lock, (), (override));
  MOCK_METHOD(void, Unlock, (), (override));
  MOCK_METHOD(bool, IsDoorOpened, (), (override));
};

// Test fixture for TimedDoor
class TimedDoorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    door_ = new TimedDoor(1);
  }

  void TearDown() override {
    delete door_;
  }

  TimedDoor* door_;
};

// Tests for TimedDoor
TEST_F(TimedDoorTest, DoorInitiallyClosed) {
  EXPECT_FALSE(door_->IsDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
  door_->Unlock();
  EXPECT_TRUE(door_->IsDoorOpened());
  door_->Lock();
}

TEST_F(TimedDoorTest, LockClosesDoor) {
  door_->Unlock();
  door_->Lock();
  EXPECT_FALSE(door_->IsDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeOutReturnsCorrectValue) {
  EXPECT_EQ(door_->GetTimeOut(), 1);
}

TEST_F(TimedDoorTest, ThrowStateThrowsException) {
  EXPECT_THROW(door_->ThrowState(), std::runtime_error);
}

// Tests for DoorTimerAdapter
TEST(DoorTimerAdapterTest, AdapterCallsThrowStateWhenDoorOpen) {
  TimedDoor door(1);
  DoorTimerAdapter adapter(door);
  door.Unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(1100));
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
  door.Lock();
}

TEST(DoorTimerAdapterTest, AdapterDoesNothingWhenDoorClosed) {
  TimedDoor door(1);
  DoorTimerAdapter adapter(door);
  door.Lock();
  EXPECT_NO_THROW(adapter.Timeout());
}

// Tests for Timer
TEST(TimerTest, TimerCallsTimeoutOnClient) {
  MockTimerClient mockClient;
  EXPECT_CALL(mockClient, Timeout()).Times(1);
  Timer timer;
  timer.TRegister(0, &mockClient);
}

TEST(TimerTest, TimerDoesNotCrashWithNullClient) {
  Timer timer;
  EXPECT_NO_THROW(timer.TRegister(0, nullptr));
}

// Integration tests
TEST(IntegrationTest, FullWorkflowWithoutTimeout) {
  TimedDoor door(1);
  door.Unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  door.Lock();
  EXPECT_FALSE(door.IsDoorOpened());
}

TEST(IntegrationTest, FullWorkflowWithTimeout) {
  TimedDoor door(1);
  door.Unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(1500));
  EXPECT_TRUE(door.IsDoorOpened());
}
