// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <chrono>
#include <stdexcept>
#include <thread>
#include "TimedDoor.h"

using ::testing::AtLeast;
using ::testing::Exactly;

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

class TimedDoorFixture : public ::testing::Test {
 protected:
  TimedDoor* door {};

  void SetUp() override {
    door = new TimedDoor(5);
  }

  void TearDown() override {
    delete door;
    door = nullptr;
  }
};

TEST_F(TimedDoorFixture, CreatedDoorIsClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorFixture, LockKeepsDoorClosed) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorFixture, UnlockOpensDoor) {
  EXPECT_THROW(door->unlock(), std::runtime_error);
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorFixture, GetTimeoutReturnsConfiguredValue) {
  EXPECT_EQ(door->getTimeOut(), 5);
}

TEST_F(TimedDoorFixture, ThrowStateThrowsWhenDoorOpened) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
  EXPECT_NO_THROW(door->throwState());

  EXPECT_THROW(door->unlock(), std::runtime_error);
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorFixture, ThrowStateDoesNotThrowWhenDoorClosed) {
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

TEST(TimerTests, RegisterCallsTimeoutOnClient) {
  Timer timer;
  MockTimerClient client;

  EXPECT_CALL(client, Timeout()).Times(Exactly(1));
  timer.tregister(0, &client);
}

TEST(TimerTests, RegisterWithNullClientDoesNothing) {
  Timer timer;
  EXPECT_NO_THROW(timer.tregister(0, nullptr));
}

TEST(DoorInterfaceTests, MockDoorMethodsCanBeInvoked) {
  MockDoor doorMock;
  EXPECT_CALL(doorMock, lock()).Times(Exactly(1));
  EXPECT_CALL(doorMock, unlock()).Times(Exactly(1));
  EXPECT_CALL(doorMock, isDoorOpened()).Times(AtLeast(1))
      .WillRepeatedly(::testing::Return(false));

  doorMock.lock();
  doorMock.unlock();
  EXPECT_FALSE(doorMock.isDoorOpened());
}

TEST(IntegrationTests, OpenDoorThenCloseBeforeTimeoutDoesNotThrow) {
  TimedDoor door(50);
  std::exception_ptr threadException;

  std::thread worker([&]() {
    try {
      door.unlock();
    } catch (...) {
      threadException = std::current_exception();
    }
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  door.lock();
  worker.join();

  EXPECT_EQ(threadException, nullptr);
  EXPECT_FALSE(door.isDoorOpened());
}

TEST(IntegrationTests, OpenDoorAndWaitForTimeoutThrows) {
  TimedDoor door(5);
  EXPECT_THROW(door.unlock(), std::runtime_error);
}
