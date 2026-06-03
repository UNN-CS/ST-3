// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <stdexcept>

#include "TimedDoor.h"

using ::testing::Return;
using ::testing::AtLeast;

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

class DoorFixture : public ::testing::Test {
 protected:
  TimedDoor* door = nullptr;
  void SetUp() override { door = new TimedDoor(7); }
  void TearDown() override { delete door; }
};

TEST_F(DoorFixture, StartsClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(DoorFixture, UnlockOpensDoor) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(DoorFixture, LockClosesDoor) {
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(DoorFixture, TimeoutValueStored) {
  EXPECT_EQ(door->getTimeOut(), 7);
}

TEST_F(DoorFixture, RepeatedUnlockKeepsOpen) {
  door->unlock();
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(DoorFixture, ThrowsWhileOpen) {
  door->unlock();
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(DoorFixture, SilentWhileClosed) {
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(DoorFixture, OpenThenCloseIsSafe) {
  door->unlock();
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(DoorFixture, ExceptionTextMatches) {
  door->unlock();
  try {
    door->throwState();
    FAIL() << "expected runtime_error";
  } catch (const std::runtime_error& e) {
    EXPECT_STREQ(e.what(), "Door has been open for too long");
  }
}

TEST(AdapterSuite, AdapterThrowsWhenDoorOpen) {
  TimedDoor d(3);
  DoorTimerAdapter adapter(d);
  d.unlock();
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(AdapterSuite, AdapterQuietWhenDoorClosed) {
  TimedDoor d(3);
  DoorTimerAdapter adapter(d);
  d.lock();
  EXPECT_NO_THROW(adapter.Timeout());
}

TEST(MockSuite, TimerInvokesTimeoutOnce) {
  MockTimerClient client;
  Timer timer;
  EXPECT_CALL(client, Timeout()).Times(1);
  timer.tregister(1, &client);
}

TEST(MockSuite, TimerDoesNotCrashOnNullClient) {
  Timer timer;
  EXPECT_NO_THROW(timer.tregister(1, nullptr));
}

TEST(MockSuite, DoorLockInvoked) {
  MockDoor mock;
  EXPECT_CALL(mock, lock()).Times(AtLeast(1));
  mock.lock();
}

TEST(MockSuite, DoorUnlockInvoked) {
  MockDoor mock;
  EXPECT_CALL(mock, unlock()).Times(1);
  mock.unlock();
}

TEST(MockSuite, DoorOpenedQueryReturnsStub) {
  MockDoor mock;
  EXPECT_CALL(mock, isDoorOpened()).WillOnce(Return(true));
  EXPECT_TRUE(mock.isDoorOpened());
}

TEST(IntegrationSuite, OpenDoorTriggersThrowThroughTimer) {
  TimedDoor d(1);
  DoorTimerAdapter adapter(d);
  Timer timer;
  d.unlock();
  EXPECT_THROW(timer.tregister(1, &adapter), std::runtime_error);
}

TEST(IntegrationSuite, ClosedDoorPassesThroughTimer) {
  TimedDoor d(1);
  DoorTimerAdapter adapter(d);
  Timer timer;
  d.lock();
  EXPECT_NO_THROW(timer.tregister(1, &adapter));
}
