// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <chrono>
#include "TimedDoor.h"

using ::testing::Invoke;
using ::testing::Mock;

class MockTimerClient : public TimerClient
{
public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class TestTimedDoor : public ::testing::Test
{
protected:
  TimedDoor *door;

  void SetUp() override
  {
    door = new TimedDoor(100);
  }

  void TearDown() override
  {
    delete door;
  }
};

TEST_F(TestTimedDoor, UnlockDoor)
{
  door->lock();
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TestTimedDoor, LockDoor)
{
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TestTimedDoor, TimeoutTest)
{
  door->unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(150));
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TestTimedDoor, TimerFunctionality)
{
  MockTimerClient mockClient;
  EXPECT_CALL(mockClient, Timeout()).Times(1);
  door->unlock();

  std::this_thread::sleep_for(std::chrono::milliseconds(150));
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TestTimedDoor, OpenCloseDoor)
{
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TestTimedDoor, StateCheckAfterOperations)
{
  door->lock();
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TestTimedDoor, OpenedDoorException)
{
  door->unlock();
  EXPECT_THROW(door->throwState(), std::runtime_error);
}
