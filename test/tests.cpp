// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdexcept>
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
  TimedDoor* door;
  void SetUp() override {
    door = new TimedDoor(10);
  }
  void TearDown() override {
    delete door;
  }
};


TEST_F(TimedDoorTest, InitialStateIsClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockSetsOpened) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockSetsClosed) {
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeoutReturnsCorrectValue) {
  EXPECT_EQ(door->getTimeOut(), 10);
}

TEST_F(TimedDoorTest, MultipleLocksStayClosed) {
  door->lock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, MultipleUnlocksStayOpened) {
  door->unlock();
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}


TEST_F(TimedDoorTest, ThrowsWhenOpened) {
  door->unlock();
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, NoThrowWhenClosed) {
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, ExceptionMessageIsCorrect) {
  door->unlock();
  try {
    door->throwState();
  } catch (const std::runtime_error& e) {
    EXPECT_STREQ(e.what(), "Door is opened too long!");
  }
}

TEST_F(TimedDoorTest, StateToggleConsistency) {
  door->unlock();
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}


TEST(MockTest, TimerCallsTimeoutOnClient) {
  MockTimerClient mockClient;
  Timer timer;
  EXPECT_CALL(mockClient, Timeout()).Times(1);
  timer.tregister(1, &mockClient);
}

TEST(MockTest, MockDoorLockCall) {
  MockDoor mDoor;
  EXPECT_CALL(mDoor, lock()).Times(1);
  mDoor.lock();
}

TEST(MockTest, MockDoorUnlockCall) {
  MockDoor mDoor;
  EXPECT_CALL(mDoor, unlock()).Times(1);
  mDoor.unlock();
}

TEST(MockTest, MockDoorOpenedStatus) {
  MockDoor mDoor;
  EXPECT_CALL(mDoor, isDoorOpened()).WillOnce(::testing::Return(true));
  EXPECT_TRUE(mDoor.isDoorOpened());
}


TEST(AdapterTest, AdapterTriggersDoorThrow) {
  TimedDoor tDoor(5);
  DoorTimerAdapter adapter(tDoor);
  tDoor.unlock();
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(AdapterTest, AdapterNoThrowIfDoorClosed) {
  TimedDoor tDoor(5);
  DoorTimerAdapter adapter(tDoor);
  tDoor.lock();
  EXPECT_NO_THROW(adapter.Timeout());
}

TEST(IntegrationTest, FullCycleOpened) {
  TimedDoor tDoor(1);
  DoorTimerAdapter adapter(tDoor);
  Timer timer;
  tDoor.unlock();
  EXPECT_THROW(timer.tregister(1, &adapter), std::runtime_error);
}

TEST(IntegrationTest, FullCycleClosed) {
  TimedDoor tDoor(1);
  DoorTimerAdapter adapter(tDoor);
  Timer timer;
  tDoor.lock();
  EXPECT_NO_THROW(timer.tregister(1, &adapter));
}