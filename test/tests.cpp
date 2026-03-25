// Copyright 2021 GHA Test Team

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <exception>
#include <stdexcept>
#include <thread>

#include "TimedDoor.h"

using ::testing::Return;
using ::testing::StrictMock;

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
  void SetUp() override {
    immediateDoor = new TimedDoor(0);
    delayedDoor = new TimedDoor(1);
  }

  void TearDown() override {
    delete delayedDoor;
    delete immediateDoor;
  }

  TimedDoor* immediateDoor{};
  TimedDoor* delayedDoor{};
};

class DoorInterfaceTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}

  DoorController controller;
};

TEST_F(TimedDoorTest, ConstructorStoresTimeoutValue) {
  EXPECT_EQ(delayedDoor->getTimeOut(), 1);
}

TEST_F(TimedDoorTest, ConstructorCreatesClosedDoor) {
  EXPECT_FALSE(delayedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoorAfterFailedUnlock) {
  EXPECT_THROW(immediateDoor->unlock(), std::runtime_error);

  immediateDoor->lock();

  EXPECT_FALSE(immediateDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockThrowsWhenDoorStaysOpened) {
  EXPECT_THROW(immediateDoor->unlock(), std::runtime_error);
}

TEST_F(TimedDoorTest, ThrowStateDoesNotThrowForClosedDoor) {
  EXPECT_NO_THROW(delayedDoor->throwState());
}

TEST_F(TimedDoorTest, ThrowStateThrowsForOpenedDoor) {
  try {
    immediateDoor->unlock();
  } catch (const std::runtime_error&) {
  }

  EXPECT_THROW(immediateDoor->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, AdapterTimeoutDoesNotThrowForClosedDoor) {
  DoorTimerAdapter adapter(*delayedDoor);

  EXPECT_NO_THROW(adapter.Timeout());
}

TEST_F(TimedDoorTest, AdapterTimeoutThrowsForOpenedDoor) {
  DoorTimerAdapter adapter(*immediateDoor);

  try {
    immediateDoor->unlock();
  } catch (const std::runtime_error&) {
  }

  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, UnlockDoesNotThrowWhenDoorGetsClosedBeforeTimeout) {
  std::exception_ptr error;

  std::thread opener([&]() {
    try {
      delayedDoor->unlock();
    } catch (...) {
      error = std::current_exception();
    }
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  delayedDoor->lock();
  opener.join();

  EXPECT_EQ(error, nullptr);
  EXPECT_FALSE(delayedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, TimerRegisterCallsClientTimeout) {
  StrictMock<MockTimerClient> client;
  Timer timer;

  EXPECT_CALL(client, Timeout()).Times(1);

  timer.tregister(0, &client);
}

TEST_F(DoorInterfaceTest, ControllerOpensDoorThroughInterface) {
  StrictMock<MockDoor> door;

  EXPECT_CALL(door, unlock()).Times(1);

  controller.Open(&door);
}

TEST_F(DoorInterfaceTest, ControllerClosesDoorThroughInterface) {
  StrictMock<MockDoor> door;

  EXPECT_CALL(door, lock()).Times(1);

  controller.Close(&door);
}

TEST_F(DoorInterfaceTest, ControllerReadsDoorStateThroughInterface) {
  StrictMock<MockDoor> door;

  EXPECT_CALL(door, isDoorOpened()).Times(1).WillOnce(Return(true));

  EXPECT_TRUE(controller.IsOpened(&door));
}

}  // namespace
