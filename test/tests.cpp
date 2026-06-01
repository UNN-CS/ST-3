// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdexcept>
#include "TimedDoor.h"

using ::testing::Return;

namespace {

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

void LockDoor(Door* door) {
  door->lock();
}

void UnlockDoor(Door* door) {
  door->unlock();
}

bool ReadDoorState(Door* door) {
  return door->isDoorOpened();
}

}  // namespace

class TimedDoorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    door = new TimedDoor(0);
  }

  void TearDown() override {
    delete door;
  }

  TimedDoor* door;
};

TEST_F(TimedDoorTest, ConstructorCreatesClosedDoor) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, ConstructorStoresTimeout) {
  EXPECT_EQ(0, door->getTimeOut());
}

TEST_F(TimedDoorTest, LockClosesDoorAfterFailedUnlock) {
  EXPECT_THROW(door->unlock(), std::runtime_error);

  door->lock();

  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockLeavesDoorOpenedWhenAlarmFires) {
  EXPECT_THROW(door->unlock(), std::runtime_error);

  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, ThrowStateDoesNotThrowForClosedDoor) {
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, ThrowStateThrowsForOpenedDoor) {
  EXPECT_THROW(door->unlock(), std::runtime_error);

  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, TimeoutThrowsWhenDoorIsOpened) {
  TimedDoor door(0);
  DoorTimerAdapter adapter(door);

  EXPECT_THROW(door.unlock(), std::runtime_error);

  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, TimeoutDoesNotThrowWhenDoorIsClosed) {
  TimedDoor door(0);
  DoorTimerAdapter adapter(door);

  EXPECT_NO_THROW(adapter.Timeout());
}

TEST(DoorTimerAdapterTest, TimerThrowsAfterDelayWhenDoorIsOpened) {
  TimedDoor door(1);

  EXPECT_THROW(door.unlock(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, TimerDoesNotThrowAfterDelayWhenDoorIsClosed) {
  TimedDoor door(1);
  DoorTimerAdapter adapter(door);
  Timer timer;

  EXPECT_NO_THROW(timer.tregister(door.getTimeOut(), &adapter));
}

TEST(TimerTest, RegisterCallsClientTimeout) {
  Timer timer;
  MockTimerClient client;

  EXPECT_CALL(client, Timeout()).Times(1);

  timer.tregister(0, &client);
}

TEST(TimerTest, RegisterAcceptsNullClient) {
  Timer timer;

  EXPECT_NO_THROW(timer.tregister(0, nullptr));
}

TEST(DoorInterfaceMockTest, LockCanBeVerifiedThroughDoorInterface) {
  MockDoor door;

  EXPECT_CALL(door, lock()).Times(1);

  LockDoor(&door);
}

TEST(DoorInterfaceMockTest, UnlockCanBeVerifiedThroughDoorInterface) {
  MockDoor door;

  EXPECT_CALL(door, unlock()).Times(1);

  UnlockDoor(&door);
}

TEST(DoorInterfaceMockTest, DoorStateCanBeMockedThroughInterface) {
  MockDoor door;

  EXPECT_CALL(door, isDoorOpened()).Times(1).WillOnce(Return(true));

  EXPECT_TRUE(ReadDoorState(&door));
}
