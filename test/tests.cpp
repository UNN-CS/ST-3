// Copyright 2021 GHA Test Team

#include <cstdint>
#include <iostream>
#include <stdexcept>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TimedDoor.h"

class MockDoorTimerAdapter : public TimerClient
{
public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door
{
public:
  MOCK_METHOD(void, lock, (), (override));
  MOCK_METHOD(void, unlock, (), (override));
  MOCK_METHOD(bool, isDoorOpened, (), (const, override));
};

class TimeDoorTests : public ::testing::Test
{
public:
  void SetUp() override { m_door = std::make_unique<TimedDoor>(); }

  void TearDown() override { m_door.reset(); }

  std::unique_ptr<TimedDoor> m_door;
};

TEST(TimeDoorTests, test_num_1)
{
  EXPECT_FALSE(m_door->isDoorOpened());
}
