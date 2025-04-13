// Copyright 2021 GHA Test Team

#include "TimedDoor.h"
#include <chrono>
#include <thread>
#include <gmock/gmock.h>
#include <gtest/gtest.h>


class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class ST3 : public ::testing::Test {
 protected:
  TimedDoor *door;
  MockTimerClient *mockClient;

  void SetUp() override {
    door = new TimedDoor(100);
    mockClient = new MockTimerClient();
  }

  void TearDown() override {
    delete door;
    delete mockClient;
  }
};

TEST_F(ST3, InitialState) { EXPECT_FALSE(door->isDoorOpened()); }

TEST_F(ST3, OpenDoor) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(ST3, CloseDoor) {
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(ST3, ExceptionOnTimeout) {
  door->unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(150));
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(ST3, NoExceptionWhileClosed) {
  door->unlock();
  door->lock();
  std::this_thread::sleep_for(std::chrono::milliseconds(150));
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(ST3, TimerTriggerTimeout) {
  Timer timer;
  EXPECT_CALL(*mockClient, Timeout()).Times(1);
  timer.tregister(100, mockClient);
  std::this_thread::sleep_for(std::chrono::milliseconds(150));
}

TEST_F(ST3, AdapterTriggerTimeout) {
  DoorTimerAdapter adapter(*door);
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(ST3, GetTimeoutVal) { EXPECT_EQ(door->getTimeOut(), 100); }

TEST_F(ST3, ReopenAfterClose) {
  door->unlock();
  door->lock();
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(ST3, CloseAgain) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}
