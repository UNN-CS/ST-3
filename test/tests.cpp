// Copyright 2025 Khasanyanov Kirill

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TimedDoor.h"
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

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
    door = new TimedDoor(1);
    adapter = new DoorTimerAdapter(*door);
    door->lock();
  }

  void TearDown() override {
    delete adapter;
    delete door;
  }

  TimedDoor *door;
  DoorTimerAdapter *adapter;
};

TEST_F(TimedDoorTest, create_closed_door) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TestLock) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TestUnlock) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  door->lock();
}

TEST_F(TimedDoorTest, TestTimeout) { EXPECT_EQ(door->getTimeOut(), 1); }

TEST_F(TimedDoorTest, TestCallTimeout) {
  MockTimerClient mockClient;
  Timer timer;
  EXPECT_CALL(mockClient, Timeout()).Times(1);
  timer.tregister(1, &mockClient);
  std::this_thread::sleep_for(2s);
}

TEST_F(TimedDoorTest, TestExceptionOpenedDoor) {

  door->unlock();
  ASSERT_ANY_THROW({
    std::this_thread::sleep_for(1.5s);
    door->lock();
  });
}

TEST_F(TimedDoorTest, TestNoExceptionClosedDoor) {
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, TestUnlockCallsTimer) {
  MockTimerClient mockClient;
  Timer timer;
  EXPECT_CALL(mockClient, Timeout()).Times(1);
  timer.tregister(1, &mockClient);
  std::this_thread::sleep_for(2s);
}

TEST_F(TimedDoorTest, TestMockDoorState) {
  MockDoor mockDoor;
  EXPECT_CALL(mockDoor, isDoorOpened()).WillOnce(::testing::Return(false));
  EXPECT_FALSE(mockDoor.isDoorOpened());
}

TEST_F(TimedDoorTest, TestMockDoorLock) {
  MockDoor mockDoor;
  EXPECT_CALL(mockDoor, lock()).Times(1);
  mockDoor.lock();
}

TEST_F(TimedDoorTest, TestMockDoorUnlock) {
  MockDoor mockDoor;
  EXPECT_CALL(mockDoor, unlock()).Times(1);
  mockDoor.unlock();
}

TEST_F(TimedDoorTest, TestFullCycle) {
  EXPECT_FALSE(door->isDoorOpened());
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
  door->unlock();
  std::this_thread::sleep_for(0.5s);
  EXPECT_NO_THROW(door->throwState());
  std::this_thread::sleep_for(1s);
  EXPECT_THROW(door->lock();, std::runtime_error);
}
