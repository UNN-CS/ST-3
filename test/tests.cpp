// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"

using ::testing::_;

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

class MockDoorTest : public ::testing::Test {
protected:
  TimedDoor *timedDoor;
  DoorTimerAdapter *adapter;
  Timer *timer;
  void SetUp() override { 
    timedDoor = new TimedDoor(5); 
    adapter = new DoorTimerAdapter(*timedDoor);
    timer = new Timer();
  }
  void TearDown() override { 
    delete timedDoor; 
  }
};

TEST_F(MockDoorTest, door_is_closed_in_the_beginning) {
  EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(MockDoorTest, door_opens) {
  timedDoor->unlock();
  EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(MockDoorTest, door_closes) {
  timedDoor->lock();
  EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(MockDoorTest, twice_opened_door_opens) {
  timedDoor->unlock();
  timedDoor->unlock();
  EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(MockDoorTest, twice_closed_door_closes) {
  timedDoor->lock();
  timedDoor->lock();
  EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(MockDoorTest, can_get_timeout) {
  EXPECT_EQ(timedDoor->getTimeOut(), 5);
}

TEST_F(MockDoorTest, can_throw_exception) {
  timedDoor->unlock();
  EXPECT_NO_THROW(adapter->Timeout());
}

TEST_F(MockDoorTest, no_exception_if_door_is_closed) {
  timedDoor->lock();
  EXPECT_NO_THROW(timedDoor->throwState());
}
TEST_F(MockDoorTest, no_exception_if_door_is_opend_and_closed) {
  timedDoor->unlock();
  timedDoor->lock();
  EXPECT_NO_THROW(timedDoor->throwState());
}

TEST(MockTimerClient, call_timeout_after_delay) {
  MockTimerClient c;
  Timer t;
  EXPECT_CALL(c, Timeout()).Times(1);
  t.tregister(1, &c);
}