// Copyright 2025 UNN-CS
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TimedDoor.h"

class MockDoor : public Door {
 public:
  MOCK_METHOD(void, lock, (), (override));
  MOCK_METHOD(void, unlock, (), (override));
  MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class MockTimedDoor : public TimedDoor {
 public:
  explicit MockTimedDoor(int timeout) : TimedDoor(timeout) {}
  MOCK_METHOD(bool, isDoorOpened, (), (override));
  MOCK_METHOD(void, throwState, (), (override));
};

class TimedDoorFixture : public ::testing::Test {
 protected:
  void SetUp() override {
    door = new TimedDoor(0);
  }

  void TearDown() override {
    delete door;
    door = nullptr;
  }

  TimedDoor* door = nullptr;
};

TEST_F(TimedDoorFixture, NewDoorIsClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorFixture, UnlockOpensDoor) {
  door->unlock();

  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorFixture, LockClosesDoor) {
  door->unlock();
  door->lock();

  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorFixture, TimeoutValueIsStored) {
  TimedDoor slowDoor(25);

  EXPECT_EQ(25, slowDoor.getTimeOut());
}

TEST_F(TimedDoorFixture, AdapterThrowsWhenDoorIsOpen) {
  door->unlock();

  EXPECT_THROW(door->triggerTimeoutForTest(), DoorTimeoutException);
}

TEST_F(TimedDoorFixture, AdapterDoesNotThrowWhenDoorIsClosed) {
  door->lock();

  EXPECT_NO_THROW(door->triggerTimeoutForTest());
}

TEST_F(TimedDoorFixture, ThrownExceptionHasMessage) {
  door->unlock();

  try {
    door->triggerTimeoutForTest();
    FAIL() << "expected DoorTimeoutException";
  } catch (const DoorTimeoutException& ex) {
    EXPECT_STREQ("door is still open after timeout", ex.what());
  }
}

TEST_F(TimedDoorFixture, TimerCallsRegisteredClient) {
  MockTimerClient client;

  EXPECT_CALL(client, Timeout()).Times(1);
  door->registerTimerForTest(0, &client);
}

TEST_F(TimedDoorFixture, TimerAllowsNullClient) {
  EXPECT_NO_THROW(door->registerTimerForTest(0, nullptr));
}

TEST_F(TimedDoorFixture, MockDoorLockCanBeVerified) {
  MockDoor mockDoor;

  EXPECT_CALL(mockDoor, lock()).Times(1);
  mockDoor.lock();
}

TEST_F(TimedDoorFixture, MockDoorUnlockCanBeVerified) {
  MockDoor mockDoor;

  EXPECT_CALL(mockDoor, unlock()).Times(1);
  mockDoor.unlock();
}

TEST_F(TimedDoorFixture, MockDoorOpenedStateCanBeVerified) {
  MockDoor mockDoor;

  EXPECT_CALL(mockDoor, isDoorOpened()).WillOnce(::testing::Return(true));
  EXPECT_TRUE(mockDoor.isDoorOpened());
}

TEST_F(TimedDoorFixture, AdapterUsesDoorInterfaceWhenOpened) {
  MockTimedDoor timedDoor(0);

  EXPECT_CALL(timedDoor, isDoorOpened()).WillOnce(::testing::Return(true));
  EXPECT_CALL(timedDoor, throwState()).Times(1).WillOnce(::testing::Invoke([] {
    throw DoorTimeoutException("mock timeout");
  }));

  EXPECT_THROW(timedDoor.triggerTimeoutForTest(), DoorTimeoutException);
}

TEST_F(TimedDoorFixture, AdapterSkipsThrowStateWhenClosed) {
  MockTimedDoor timedDoor(0);

  EXPECT_CALL(timedDoor, isDoorOpened()).WillOnce(::testing::Return(false));
  EXPECT_CALL(timedDoor, throwState()).Times(0);

  EXPECT_NO_THROW(timedDoor.triggerTimeoutForTest());
}
