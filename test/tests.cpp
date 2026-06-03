// Copyright 2021 GHA Test Team

#include <gmock/gmock.h>
#include <gtest/gtest.h>

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
  void SetUp() override {
    door = new TimedDoor(1);
  }

  void TearDown() override {
    delete door;
  }

  TimedDoor* door;
};

TEST_F(TimedDoorTest, DoorIsClosedAfterConstruction) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeOutReturnsConstructorValue) {
  EXPECT_EQ(door->getTimeOut(), 1);
}

TEST_F(TimedDoorTest, LockClosesTheDoor) {
  EXPECT_THROW(door->unlock(), std::runtime_error);

  door->lock();

  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensTheDoor) {
  EXPECT_THROW(door->unlock(), std::runtime_error);

  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, IsDoorOpenedReturnsTrueAfterUnlock) {
  EXPECT_THROW(door->unlock(), std::runtime_error);

  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, ThrowStateThrowsWhenDoorIsOpen) {
  EXPECT_THROW(door->unlock(), std::runtime_error);

  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, ThrowStateDoesNotThrowWhenDoorIsClosed) {
  door->lock();

  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, DoorTimerAdapterTimeoutThrowsWhenDoorIsOpen) {
  EXPECT_THROW(door->unlock(), std::runtime_error);

  DoorTimerAdapter adapter(*door);

  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, DoorTimerAdapterTimeoutDoesNotThrowWhenDoorIsClosed) {
  DoorTimerAdapter adapter(*door);

  EXPECT_NO_THROW(adapter.Timeout());
}

TEST_F(TimedDoorTest, TimerRegisterDoesNotCallClientTimeoutDirectly) {
  MockTimerClient client;
  Timer timer;

  EXPECT_CALL(client, Timeout()).Times(0);

  timer.tregister(0, &client);
}

TEST_F(TimedDoorTest, TimerClientInterfaceCanBeMocked) {
  MockTimerClient client;

  EXPECT_CALL(client, Timeout()).Times(1);

  client.Timeout();
}

TEST_F(TimedDoorTest, DoorInterfaceCanBeMocked) {
  MockDoor mockDoor;

  EXPECT_CALL(mockDoor, unlock()).Times(1);
  EXPECT_CALL(mockDoor, isDoorOpened())
      .Times(1)
      .WillOnce(testing::Return(true));
  EXPECT_CALL(mockDoor, lock()).Times(1);

  mockDoor.unlock();
  EXPECT_TRUE(mockDoor.isDoorOpened());
  mockDoor.lock();
}
