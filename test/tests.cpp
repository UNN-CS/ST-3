// Copyright 2021 GHA Test Team

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <stdexcept>

#include "TimedDoor.h"

using ::testing::InSequence;
using ::testing::Return;

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

static void OpenAndCloseIfOpened(Door& door) {
  door.unlock();
  if (door.isDoorOpened()) {
    door.lock();
  }
}

class TimedDoorFixture : public ::testing::Test {
 protected:
  TimedDoor* fastDoor;
  TimedDoor* longDoor;

  void SetUp() override {
    fastDoor = new TimedDoor(0);
    longDoor = new TimedDoor(2);
  }

  void TearDown() override {
    delete fastDoor;
    delete longDoor;
  }
};

TEST_F(TimedDoorFixture, ConstructorStoresTimeoutValue) {
  EXPECT_EQ(longDoor->getTimeOut(), 2);
}

TEST_F(TimedDoorFixture, DoorIsClosedAfterConstruction) {
  EXPECT_FALSE(fastDoor->isDoorOpened());
}

TEST_F(TimedDoorFixture, LockChangesStateToClosed) {
  EXPECT_THROW(fastDoor->unlock(), std::runtime_error);
  EXPECT_TRUE(fastDoor->isDoorOpened());

  fastDoor->lock();
  EXPECT_FALSE(fastDoor->isDoorOpened());
}

TEST_F(TimedDoorFixture, ThrowStateAlwaysThrowsRuntimeError) {
  EXPECT_THROW(fastDoor->throwState(), std::runtime_error);
}

TEST_F(TimedDoorFixture, AdapterTimeoutThrowsWhenDoorOpened) {
  TimedDoor door(0);
  DoorTimerAdapter adapter(door);

  door.lock();
  EXPECT_FALSE(door.isDoorOpened());
  EXPECT_NO_THROW(adapter.Timeout());

  EXPECT_THROW(door.unlock(), std::runtime_error);
  EXPECT_TRUE(door.isDoorOpened());
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(TimedDoorFixture, AdapterTimeoutDoesNotThrowWhenDoorClosed) {
  TimedDoor door(0);
  DoorTimerAdapter adapter(door);

  door.lock();
  EXPECT_FALSE(door.isDoorOpened());
  EXPECT_NO_THROW(adapter.Timeout());
}

TEST_F(TimedDoorFixture, UnlockThrowsForZeroTimeoutOpenedDoor) {
  EXPECT_THROW(fastDoor->unlock(), std::runtime_error);
}

TEST(TimerTests, RegisterCallsTimeoutOnClient) {
  Timer timer;
  MockTimerClient client;

  EXPECT_CALL(client, Timeout()).Times(1);
  EXPECT_NO_THROW(timer.tregister(0, &client));
}

TEST(TimerTests, RegisterWithNullClientDoesNotThrow) {
  Timer timer;
  EXPECT_NO_THROW(timer.tregister(0, nullptr));
}

TEST(DoorInterfaceTests, OpenAndCloseIfOpenedCallsMethodsInOrder) {
  MockDoor door;
  InSequence seq;

  EXPECT_CALL(door, unlock()).Times(1);
  EXPECT_CALL(door, isDoorOpened()).WillOnce(Return(true));
  EXPECT_CALL(door, lock()).Times(1);

  OpenAndCloseIfOpened(door);
}

TEST(DoorInterfaceTests, OpenAndCloseIfOpenedDoesNotLockIfAlreadyClosed) {
  MockDoor door;

  EXPECT_CALL(door, unlock()).Times(1);
  EXPECT_CALL(door, isDoorOpened()).WillOnce(Return(false));
  EXPECT_CALL(door, lock()).Times(0);

  OpenAndCloseIfOpened(door);
}
