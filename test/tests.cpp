// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <chrono>
#include <exception>
#include <memory>
#include <thread>

#include "TimedDoor.h"

using ::testing::StrictMock;
using std::chrono_literals::operator""ms;

namespace {

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    instantDoor = std::make_unique<TimedDoor>(0);
    delayedDoor = std::make_unique<TimedDoor>(1);
  }

  void TearDown() override {
    instantDoor.reset();
    delayedDoor.reset();
  }

  std::unique_ptr<TimedDoor> instantDoor;
  std::unique_ptr<TimedDoor> delayedDoor;
};

TEST_F(TimedDoorTest, DoorIsClosedAfterConstruction) {
  EXPECT_FALSE(instantDoor->isDoorOpened());
  EXPECT_FALSE(delayedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeoutReturnsConstructorValue) {
  EXPECT_EQ(0, instantDoor->getTimeOut());
  EXPECT_EQ(1, delayedDoor->getTimeOut());
}

TEST_F(TimedDoorTest, LockKeepsDoorClosed) {
  delayedDoor->lock();

  EXPECT_FALSE(delayedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, ThrowStateDoesNotThrowForClosedDoor) {
  delayedDoor->lock();

  EXPECT_NO_THROW(delayedDoor->throwState());
}

TEST_F(TimedDoorTest, ThrowStateThrowsForOpenedDoor) {
  std::exception_ptr asyncException;
  std::thread worker([&] {
    try {
      delayedDoor->unlock();
    } catch (...) {
      asyncException = std::current_exception();
    }
  });

  std::this_thread::sleep_for(100ms);

  EXPECT_THROW(delayedDoor->throwState(), std::runtime_error);

  delayedDoor->lock();
  worker.join();
  EXPECT_EQ(nullptr, asyncException);
}

TEST_F(TimedDoorTest, UnlockMarksDoorAsOpenedBeforeTimeoutExpires) {
  std::exception_ptr asyncException;
  std::thread worker([&] {
    try {
      delayedDoor->unlock();
    } catch (...) {
      asyncException = std::current_exception();
    }
  });

  std::this_thread::sleep_for(100ms);

  EXPECT_TRUE(delayedDoor->isDoorOpened());

  delayedDoor->lock();
  worker.join();
  EXPECT_EQ(nullptr, asyncException);
}

TEST_F(TimedDoorTest, UnlockThrowsWhenDoorRemainsOpened) {
  EXPECT_THROW(instantDoor->unlock(), std::runtime_error);
}

TEST_F(TimedDoorTest, UnlockLeavesDoorOpenedAfterTimeoutException) {
  EXPECT_THROW(instantDoor->unlock(), std::runtime_error);

  EXPECT_TRUE(instantDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockDoesNotThrowWhenDoorIsLockedBeforeTimeout) {
  std::exception_ptr asyncException;
  std::thread worker([&] {
    try {
      delayedDoor->unlock();
    } catch (...) {
      asyncException = std::current_exception();
    }
  });

  std::this_thread::sleep_for(100ms);
  delayedDoor->lock();
  worker.join();

  EXPECT_EQ(nullptr, asyncException);
  EXPECT_FALSE(delayedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, AdapterThrowsForOpenedDoor) {
  DoorTimerAdapter adapter(*delayedDoor);
  std::exception_ptr asyncException;
  std::thread worker([&] {
    try {
      delayedDoor->unlock();
    } catch (...) {
      asyncException = std::current_exception();
    }
  });

  std::this_thread::sleep_for(100ms);

  EXPECT_THROW(adapter.Timeout(), std::runtime_error);

  delayedDoor->lock();
  worker.join();
  EXPECT_EQ(nullptr, asyncException);
}

TEST_F(TimedDoorTest, AdapterDoesNotThrowForClosedDoor) {
  DoorTimerAdapter adapter(*instantDoor);
  instantDoor->lock();

  EXPECT_NO_THROW(adapter.Timeout());
}

TEST(TimerTest, RegisterInvokesTimeoutOnClient) {
  StrictMock<MockTimerClient> client;
  Timer timer;

  EXPECT_CALL(client, Timeout()).Times(1);

  timer.tregister(0, &client);
}

TEST(TimerTest, RegisterWithNullClientDoesNotThrow) {
  Timer timer;

  EXPECT_NO_THROW(timer.tregister(0, nullptr));
}

}  // namespace
