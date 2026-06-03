// Copyright 2021 GHA Test Team

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <stdexcept>

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

class QuietTimedDoor : public TimedDoor {
 public:
  explicit QuietTimedDoor(int timeout) : TimedDoor(timeout), alarms(0) {}

  void throwState() override { ++alarms; }

  int alarms;
};

class TimedDoorTest : public ::testing::Test {
 protected:
  void SetUp() override { door = new TimedDoor(0); }

  void TearDown() override {
    delete door;
    door = nullptr;
  }

  TimedDoor *door;
};

TEST_F(TimedDoorTest, DoorStartsClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockKeepsClosedDoorClosed) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutValueIsStored) {
  TimedDoor slowDoor(25);
  EXPECT_EQ(25, slowDoor.getTimeOut());
}

TEST_F(TimedDoorTest, ManualTimeoutOnClosedDoorDoesNotThrow) {
  door->lock();
  EXPECT_NO_THROW(door->triggerTimeoutForTest());
}

TEST_F(TimedDoorTest, ManualTimeoutOnOpenDoorThrows) {
  QuietTimedDoor openDoor(0);
  openDoor.unlock();

  TimedDoor strictDoor(0);
  EXPECT_THROW(strictDoor.unlock(), DoorTimeoutException);
}

TEST_F(TimedDoorTest, UnlockLeavesDoorOpenedWhenAlarmIsHandled) {
  QuietTimedDoor quietDoor(0);
  quietDoor.unlock();
  EXPECT_TRUE(quietDoor.isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockCallsAlarmWhenDoorRemainsOpen) {
  QuietTimedDoor quietDoor(0);
  quietDoor.unlock();
  EXPECT_EQ(1, quietDoor.alarms);
}

TEST_F(TimedDoorTest, LockAfterHandledAlarmClosesDoor) {
  QuietTimedDoor quietDoor(0);
  quietDoor.unlock();
  quietDoor.lock();
  EXPECT_FALSE(quietDoor.isDoorOpened());
}

TEST_F(TimedDoorTest, ThrowStateHasExpectedExceptionType) {
  EXPECT_THROW(door->throwState(), DoorTimeoutException);
}

TEST_F(TimedDoorTest, ThrowStateMessageDescribesExpiredDoor) {
  try {
    door->throwState();
    FAIL() << "DoorTimeoutException was expected";
  } catch (const DoorTimeoutException &error) {
    EXPECT_STREQ("Door remained open past timeout", error.what());
  }
}

TEST_F(TimedDoorTest, TimerCallsRegisteredClient) {
  MockTimerClient client;
  EXPECT_CALL(client, Timeout()).Times(1);
  door->registerTimerForTest(0, &client);
}

TEST_F(TimedDoorTest, TimerAllowsEmptyClient) {
  EXPECT_NO_THROW(door->registerTimerForTest(0, nullptr));
}

TEST_F(TimedDoorTest, TimerRejectsNegativeTimeout) {
  MockTimerClient client;
  EXPECT_THROW(door->registerTimerForTest(-1, &client), std::invalid_argument);
}

TEST_F(TimedDoorTest, MockDoorLockCanBeVerified) {
  MockDoor mockDoor;
  EXPECT_CALL(mockDoor, lock()).Times(1);
  mockDoor.lock();
}

TEST_F(TimedDoorTest, MockDoorUnlockCanBeVerified) {
  MockDoor mockDoor;
  EXPECT_CALL(mockDoor, unlock()).Times(1);
  mockDoor.unlock();
}

TEST_F(TimedDoorTest, MockDoorStateCanBeVerified) {
  MockDoor mockDoor;
  EXPECT_CALL(mockDoor, isDoorOpened()).WillOnce(testing::Return(true));
  EXPECT_TRUE(mockDoor.isDoorOpened());
}

TEST_F(TimedDoorTest, AdapterChecksDoorStateBeforeAlarm) {
  MockTimedDoor mockDoor(0);
  EXPECT_CALL(mockDoor, isDoorOpened()).WillOnce(testing::Return(false));
  EXPECT_CALL(mockDoor, throwState()).Times(0);
  EXPECT_NO_THROW(mockDoor.triggerTimeoutForTest());
}

TEST_F(TimedDoorTest, AdapterThrowsWhenMockDoorIsOpen) {
  MockTimedDoor mockDoor(0);
  EXPECT_CALL(mockDoor, isDoorOpened()).WillOnce(testing::Return(true));
  EXPECT_CALL(mockDoor, throwState()).WillOnce(testing::Throw(
      DoorTimeoutException("mocked timeout")));

  EXPECT_THROW(mockDoor.triggerTimeoutForTest(), DoorTimeoutException);
}
