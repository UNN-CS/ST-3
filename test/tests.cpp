// Copyright 2025 tyurinm
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"
#include <stdexcept>
#include <thread>
#include <chrono>

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
  MockTimerClient* mockClient;

  void SetUp() override {
    mockClient = new MockTimerClient();
    door = new TimedDoor(5);
  }

  void TearDown() override {
    delete mockClient;
    delete door;
  }
};

TEST_F(TimedDoorTest, DoorClosedAfterLock) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TimerCorrectSleep) {
  Timer timer;
  MockTimerClient mockClient;
  EXPECT_CALL(mockClient, Timeout()).Times(1);
  timer.tregister(5, &mockClient);
  std::this_thread::sleep_for(std::chrono::seconds(6));
}

TEST_F(TimedDoorTest, InvalidTimerRegistration) {
  Timer timer;
  EXPECT_THROW(timer.tregister(5, nullptr), std::invalid_argument);
  EXPECT_THROW(timer.tregister(-1, mockClient), std::invalid_argument);
}

TEST_F(TimedDoorTest, TimerWithDelayedExecution) {
  Timer timer;
  EXPECT_CALL(*mockClient, Timeout()).Times(1);
  timer.tregister(3, mockClient);
  std::this_thread::sleep_for(std::chrono::seconds(4));
}

TEST_F(TimedDoorTest, LargeTimerDuration) {
  Timer timer;
  MockTimerClient mockClient;
  EXPECT_CALL(mockClient, Timeout()).Times(1);
  timer.tregister(100, &mockClient);
  std::this_thread::sleep_for(std::chrono::seconds(101));
}

TEST_F(TimedDoorTest, MultipleTimeouts) {
  Timer timer;
  MockTimerClient mockClient;
  EXPECT_CALL(mockClient, Timeout()).Times(2);
  timer.tregister(3, &mockClient);
  std::this_thread::sleep_for(std::chrono::seconds(4));
  timer.tregister(3, &mockClient);
  std::this_thread::sleep_for(std::chrono::seconds(4));
}

TEST_F(TimedDoorTest, AsyncTimerExecution) {
  Timer timer;
  MockTimerClient mockClient;
  EXPECT_CALL(mockClient, Timeout()).Times(1);
  std::thread t([&timer, &mockClient]() {
    timer.tregister(3, &mockClient);
  });
  t.join();
  std::this_thread::sleep_for(std::chrono::seconds(4));
}

TEST_F(TimedDoorTest, DoorRemainsOpenedBeforeTimeout) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  std::this_thread::sleep_for(std::chrono::seconds(3));
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, RepeatedUnlockDoesNotThrow) {
  door->unlock();
  EXPECT_NO_THROW(door->unlock());
}

TEST_F(TimedDoorTest, TimerThrowsExceptionForNullClient) {
  Timer timer;

  EXPECT_THROW(timer.tregister(5, nullptr), std::invalid_argument);
}

TEST_F(TimedDoorTest, UnlockAndImmediateLock) {
  door->unlock();
  std::this_thread::sleep_for(std::chrono::seconds(2));

  door->lock();
  EXPECT_FALSE(door->isDoorOpened());

  EXPECT_CALL(*mockClient, Timeout()).Times(0);
}
