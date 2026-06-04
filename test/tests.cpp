// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
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
  MOCK_METHOD(void, lock,         (), (override));
  MOCK_METHOD(void, unlock,       (), (override));
  MOCK_METHOD(bool, isDoorOpened, (), (override));
};

static void openThenClose(Door& door) {
  door.unlock();
  if (door.isDoorOpened())
    door.lock();
}

class DoorFixture : public ::testing::Test {
 protected:
  TimedDoor* door0;
  TimedDoor* door2;
  void SetUp() override {
    door0 = new TimedDoor(0);
    door2 = new TimedDoor(2);
  }
  void TearDown() override {
    delete door0;
    delete door2;
  }
};

TEST_F(DoorFixture, initiallyClose) {
  EXPECT_FALSE(door0->isDoorOpened());
}
TEST_F(DoorFixture, timeoutStored) {
  EXPECT_EQ(0, door0->getTimeOut());
  EXPECT_EQ(2, door2->getTimeOut());
}
TEST_F(DoorFixture, throwStateThrows) {
  EXPECT_THROW(door0->throwState(), std::runtime_error);
}
TEST_F(DoorFixture, lockClosesDoor) {
  EXPECT_THROW(door0->unlock(), std::runtime_error);
  EXPECT_TRUE(door0->isDoorOpened());
  door0->lock();
  EXPECT_FALSE(door0->isDoorOpened());
}
TEST_F(DoorFixture, unlockZeroThrows) {
  EXPECT_THROW(door0->unlock(), std::runtime_error);
}
TEST_F(DoorFixture, multipleLocksOk) {
  door0->lock();
  door0->lock();
  EXPECT_FALSE(door0->isDoorOpened());
}

TEST(Adapter, noThrowWhenClosed) {
  TimedDoor d(0);
  DoorTimerAdapter a(d);
  EXPECT_NO_THROW(a.Timeout());
}
TEST(Adapter, throwsWhenOpen) {
  TimedDoor d(0);
  DoorTimerAdapter a(d);
  EXPECT_THROW(d.unlock(), std::runtime_error);
  EXPECT_THROW(a.Timeout(), std::runtime_error);
}
TEST(Adapter, lockPreventsThrow) {
  TimedDoor d(0);
  DoorTimerAdapter a(d);
  EXPECT_THROW(d.unlock(), std::runtime_error);
  d.lock();
  EXPECT_NO_THROW(a.Timeout());
}

TEST(Timer, callsTimeout) {
  Timer t;
  MockTimerClient mc;
  EXPECT_CALL(mc, Timeout()).Times(1);
  EXPECT_NO_THROW(t.tregister(0, &mc));
}
TEST(Timer, nullptrSafe) {
  Timer t;
  EXPECT_NO_THROW(t.tregister(0, nullptr));
}

TEST(DoorMock, openAndClose) {
  MockDoor md;
  InSequence seq;
  EXPECT_CALL(md, unlock()).Times(1);
  EXPECT_CALL(md, isDoorOpened()).WillOnce(Return(true));
  EXPECT_CALL(md, lock()).Times(1);
  openThenClose(md);
}
TEST(DoorMock, noLockWhenClosed) {
  MockDoor md;
  EXPECT_CALL(md, unlock()).Times(1);
  EXPECT_CALL(md, isDoorOpened()).WillOnce(Return(false));
  EXPECT_CALL(md, lock()).Times(0);
  openThenClose(md);
}
TEST(DoorMock, isDoorOpenedCalled) {
  MockDoor md;
  EXPECT_CALL(md, unlock()).Times(1);
  EXPECT_CALL(md, isDoorOpened()).WillOnce(Return(true));
  EXPECT_CALL(md, lock()).Times(1);
  openThenClose(md);
}
