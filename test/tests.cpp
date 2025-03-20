// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"
#include <thread>

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
  MockTimerClient client;
  EXPECT_CALL(client, Timeout()).WillOnce(Invoke([this]()
                                                 { door->throwState(); }));
  door->unlock();
  EXPECT_THROW(client.Timeout(), std::runtime_error);
}

TEST_F(TestTimedDoor, TimerFunctionality)
{
  MockTimerClient mockClient;
  EXPECT_CALL(mockClient, Timeout()).Times(1);
  door->unlock();
  door->lock();
}

TEST_F(TestTimedDoor, OpenCloseDoor)
{
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TestTimedDoor, DifferentTimeouts)
{
  TimedDoor shortDoor(50);
  EXPECT_NO_THROW(shortDoor.unlock());
  std::this_thread::sleep_for(std::chrono::milliseconds(60));
  EXPECT_THROW(shortDoor.throwState(), std::runtime_error);
}

TEST_F(TestTimedDoor, MultipleUnlocks)
{
  for (int i = 0; i < 10; ++i)
  {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
  }
}

TEST_F(TestTimedDoor, StateCheckAfterOperations)
{
  door->lock();
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TestTimedDoor, AdapterCreation)
{
  EXPECT_NO_THROW(DoorTimerAdapter adapter(*door));
}

TEST_F(TestTimedDoor, OpenedDoorException)
{
  door->unlock();
  EXPECT_THROW(door->throwState(), std::runtime_error);
}
