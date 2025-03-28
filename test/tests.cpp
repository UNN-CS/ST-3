// Copyright 2021 GHA Test Team

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <thread>
#include "TimedDoor.h"


class MockableTimer {
 public:
  virtual ~MockableTimer() = default;
  virtual void tregister(int, TimerClient *) = 0;
};

class MockTimer : public MockableTimer {
 public:
  MOCK_METHOD(void, tregister, (int, TimerClient *), (override));
};

class MockDoorTimerAdapter : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door {
 public:
  MOCK_METHOD(void, lock, (), (override));
  MOCK_METHOD(void, unlock, (), (override));
  MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class TimedTest : public ::testing::Test {
 protected:
  void SetUp() override { door = new TimedDoor(10); }
  void TearDown() override { delete door; }
  TimedDoor *door;
};

TEST_F(TimedTest, Create_time_test) {
  EXPECT_FALSE(door->isDoorOpened());
  EXPECT_EQ(door->getTimeOut(), 10);
}

TEST_F(TimedTest, Create_door_lock_test) {
  EXPECT_NO_THROW(door->unlock());
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedTest, Create_door_lock2_throw_test) {
  if (door->isDoorOpened())
    door->lock();
  EXPECT_ANY_THROW(door->lock());
}

TEST_F(TimedTest, Create_door_unlock_test) {
  if (!door->isDoorOpened())
    door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}
TEST_F(TimedTest, Create_door_unlock2_throw_test) {
  if (!door->isDoorOpened())
    door->unlock();
  EXPECT_ANY_THROW(door->unlock());
}

TEST_F(TimedTest, Timer_door_timethrow_test) {
  sleep(10);
  EXPECT_ANY_THROW(door->throwState());
}

class DoorTimerAdapterTest : public ::testing::Test {
 protected:
  MockTimer *timer;
  void SetUp() override { timer = new MockTimer(); }
  void TearDown() override { delete timer; }
};

class TimerClientTest : public ::testing::Test {
 protected:
  void SetUp() override {
    door = new TimedDoor(10);
    adapter = new DoorTimerAdapter(*door);
  }
  void TearDown() override {
    delete adapter;
    delete door;
  }
  TimedDoor *door;
  DoorTimerAdapter *adapter;
};

TEST_F(TimerClientTest, TimerAdapterTimeout_throw_long_open_test) {
  if (!door->isDoorOpened())
    door->unlock();
  EXPECT_ANY_THROW(adapter->Timeout());
}

TEST_F(TimerClientTest, TimerAdapterTimeout_nothrow_long_closed_test) {
  if (door->isDoorOpened())
    door->lock();
  EXPECT_NO_THROW(adapter->Timeout());
}

class TimerAdapterTest : public ::testing::Test {
 protected:
  void SetUp() override { door = new TimedDoor(1); }
  void TearDown() override { delete door; }
  TimedDoor *door;
};

TEST_F(TimerAdapterTest, TimerAdapter_time_locked_test) {
  if (door->isDoorOpened())
    door->lock();
  EXPECT_FALSE(door->isDoorOpened());
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  DoorTimerAdapter adapter(*door);
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
  if (door->isDoorOpened())
    door->lock();
  EXPECT_FALSE(door->isDoorOpened());
  EXPECT_NO_THROW(adapter.Timeout());
}

TEST_F(TimerAdapterTest, TimerAdapter_time_open_test) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
  DoorTimerAdapter adapter(*door);
  EXPECT_NO_THROW(adapter.Timeout());
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}
