// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <cstdint>
#include "TimedDoor.h"

using ::testing::Return;

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

class DoorController {
 public:
  void Open(Door* door) {
    door->unlock();
  }

  void Close(Door* door) {
    door->lock();
  }

  bool IsOpened(Door* door) {
    return door->isDoorOpened();
  }
};

class TimedDoorTest : public ::testing::Test {
 protected:
  TimedDoor* door{};
  DoorTimerAdapter* adapter{};
  Timer* timer{};
  DoorController* controller{};

  void SetUp() override {
    door = new TimedDoor(25);
    adapter = new DoorTimerAdapter(*door);
    timer = new Timer();
    controller = new DoorController();
  }

  void TearDown() override {
    delete controller;
    delete timer;
    delete adapter;
    delete door;
  }
};

TEST_F(TimedDoorTest, DoorIsClosedAfterConstruction) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
  door->unlock();

  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesOpenedDoor) {
  door->unlock();
  door->lock();

  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeoutReturnsConstructorValue) {
  EXPECT_EQ(25, door->getTimeOut());
}

TEST_F(TimedDoorTest, ThrowStateThrowsForOpenedDoor) {
  door->unlock();

  EXPECT_ANY_THROW(door->throwState());
}

TEST_F(TimedDoorTest, ThrowStateDoesNotThrowForClosedDoor) {
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, AdapterThrowsWhenDoorIsStillOpened) {
  door->unlock();

  EXPECT_ANY_THROW(adapter->Timeout());
}

TEST_F(TimedDoorTest, AdapterDoesNotThrowWhenDoorIsClosed) {
  EXPECT_NO_THROW(adapter->Timeout());
}

TEST_F(TimedDoorTest, TimerCallsClientTimeoutOnce) {
  MockTimerClient client;

  EXPECT_CALL(client, Timeout()).Times(1);

  timer->tregister(1, &client);
}

TEST_F(TimedDoorTest, TimerWaitsSpecifiedTimeoutBeforeCallback) {
  MockTimerClient client;
  const auto start = std::chrono::steady_clock::now();

  EXPECT_CALL(client, Timeout()).Times(1);

  timer->tregister(20, &client);

  const auto finish = std::chrono::steady_clock::now();
  const auto elapsed =
      std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

  EXPECT_GE(elapsed.count(), 15);
}

TEST_F(TimedDoorTest, ControllerUsesDoorInterfaceToOpenDoor) {
  MockDoor mockDoor;

  EXPECT_CALL(mockDoor, unlock()).Times(1);

  controller->Open(&mockDoor);
}

TEST_F(TimedDoorTest, ControllerUsesDoorInterfaceToCloseDoor) {
  MockDoor mockDoor;

  EXPECT_CALL(mockDoor, lock()).Times(1);

  controller->Close(&mockDoor);
}

TEST_F(TimedDoorTest, ControllerReadsDoorStateThroughInterface) {
  MockDoor mockDoor;

  EXPECT_CALL(mockDoor, isDoorOpened()).Times(1).WillOnce(Return(true));

  EXPECT_TRUE(controller->IsOpened(&mockDoor));
}
