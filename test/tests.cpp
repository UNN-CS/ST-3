// Copyright 2025 ovVrLFg8ks

#include <gmock/gmock.h>
#include <gtest/gtest.h>

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
  DoorTimerAdapter* adapter;
  TimedDoor* door;
  Timer* timer;

  void SetUp() override {
    door = new TimedDoor(1000);
    timer = new Timer();
    adapter = new DoorTimerAdapter(*door);
  }

  void TearDown() override {
    delete adapter;
    delete door;
    delete timer;
  }
};

TEST_F(TimedDoorTest, DoorClosedInit) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorCloseAfterInit) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorOpenCloseCycle) {
  for (int i = 0; i < 9; i++) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
  }
}

TEST_F(TimedDoorTest, GetTimeout) {
  EXPECT_EQ(door->getTimeOut(), 1000);
}

TEST_F(TimedDoorTest, DirectThrow) {
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, TimeoutThrow) {
  door->unlock();
  EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, TimeoutThrowFR) {
  door->unlock();
  EXPECT_THROW(timer->tregister(door->getTimeOut(), adapter), std::runtime_error);
}

TEST_F(TimedDoorTest, CloseBeforeTimer) {
  door->unlock();
  door->lock();
  EXPECT_NO_THROW(timer->tregister(door->getTimeOut(), adapter));
}

TEST(MockTimerTest, TimeoutCalls) {
  MockTimerClient timerClient;
  Timer timer;

  EXPECT_CALL(timerClient, Timeout()).Times(1);
  timer.tregister(200, &timerClient);
}

TEST(MockTimerTest, TimeoutCallsNoStart) {
  MockTimerClient timerClient;
  Timer timer;

  EXPECT_CALL(timerClient, Timeout()).Times(0);
}

TEST(MockDoorTest, DoorOpenCloseCycleCalls) {
  MockDoor door;
  EXPECT_CALL(door, unlock()).Times(9);
  EXPECT_CALL(door, lock()).Times(9);
  for (int i = 0; i < 9; i++) {
    door.unlock();
    door.lock();
  }
}
