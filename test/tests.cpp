// Copyright 2021 GHA Test Team

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <stdexcept>
#include <memory>

#include "TimedDoor.h"

class MockDoorTimerAdapter : public TimerClient{
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door{
 public:
  MOCK_METHOD(void, lock, (), (override));
  MOCK_METHOD(void, unlock, (), (override));
  MOCK_METHOD(bool, isDoorOpened, (), (const, override));
};

class TimeDoorTests : public ::testing::Test{
 protected:
  std::unique_ptr<TimedDoor> m_door;

  void SetUp() override { m_door = std::make_unique<TimedDoor>(2); }

  void TearDown() override { m_door.reset(); }
};

TEST_F(TimeDoorTests, test_num_1) {
  EXPECT_FALSE(m_door->isDoorOpened());
}

TEST_F(TimeDoorTests, test_num_2) {
  m_door->unlock();
  EXPECT_TRUE(m_door->isDoorOpened());
}

TEST_F(TimeDoorTests, test_num_3) {
  m_door->unlock();
  m_door->lock();
  EXPECT_FALSE(m_door->isDoorOpened());
}

TEST_F(TimeDoorTests, test_num_4) {
  m_door->lock();
  EXPECT_THROW(m_door->isDoorOpened(), std::runtime_error);
}

TEST_F(TimeDoorTests, test_num_5) {
	m_door->unlock();
	EXPECT_THROW(m_door->unlock(), std::runtime_error);
}

TEST_F(TimeDoorTests, test_num_6) {
  EXPECT_THROW(m_door->throwState(), std::runtime_error);
}

TEST_F(TimeDoorTests, test_num_7) {
  EXPECT_EQ(m_door->getTimeOut(), 2);
}

TEST_F(TimeDoorTests, test_num_8) {
	m_door->unlock();
	m_door->throwState();
	EXPECT_TRUE(m_door->isDoorOpened());
}

TEST_F(TimeDoorTests, test_num_9) {
	m_door->unlock();
	m_door->throwState();
	std::this_thread::sleep_for(std::chrono::seconds(2));
	EXPECT_THROW(m_door->throwState(), std::runtime_error);
}

TEST_F(TimeDoorTests, test_num_10) {
	m_door->unlock();
	m_door->throwState();
	std::this_thread::sleep_for(std::chrono::seconds(3));
	EXPECT_THROW(m_door->throwState(), std::runtime_error);
}