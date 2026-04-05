// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <chrono>
#include <exception>
#include <memory>
#include <stdexcept>
#include <thread>

#include "TimedDoor.h"

using ::testing::Return;

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
    door_ = std::make_unique<TimedDoor>(0);
    door_->lock();
  }

  void TearDown() override {
    door_.reset();
  }

  std::unique_ptr<TimedDoor> door_;
};

TEST_F(TimedDoorTest, StartsClosedAfterLock) {
  EXPECT_FALSE(door_->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeOutReturnsConfiguredValue) {
  TimedDoor door(42);
  EXPECT_EQ(door.getTimeOut(), 42);
}

TEST_F(TimedDoorTest, ThrowStateDoesNotThrowWhenClosed) {
  EXPECT_NO_THROW(door_->throwState());
}

TEST_F(TimedDoorTest, UnlockWithZeroDelayThrowsWhenStillOpen) {
  EXPECT_THROW(door_->unlock(), std::runtime_error);
}

TEST_F(TimedDoorTest, LockIsIdempotent) {
  door_->lock();
  door_->lock();
  EXPECT_FALSE(door_->isDoorOpened());
}

TEST(TimerTest, TRegisterCallsTimeoutAfterSleep) {
  MockTimerClient mock;
  Timer timer;
  EXPECT_CALL(mock, Timeout()).Times(1);
  timer.tregister(0, &mock);
}

TEST(TimerTest, TRegisterSkipsTimeoutWhenClientIsNull) {
  Timer timer;
  timer.tregister(0, nullptr);
}

TEST(MockDoorTest, LockInvokesMock) {
  MockDoor mock;
  EXPECT_CALL(mock, lock()).Times(1);
  mock.lock();
}

TEST(MockDoorTest, UnlockInvokesMock) {
  MockDoor mock;
  EXPECT_CALL(mock, unlock()).Times(1);
  mock.unlock();
}

TEST(MockDoorTest, IsDoorOpenedReturnsStubbedValue) {
  MockDoor mock;
  EXPECT_CALL(mock, isDoorOpened()).WillOnce(Return(true));
  EXPECT_TRUE(mock.isDoorOpened());
}

TEST(TimedDoorIntegrationTest, UnlockDoesNotThrowIfLockedBeforeTimeout) {
  TimedDoor door(1);
  door.lock();
  std::exception_ptr eptr;
  std::thread worker([&door, &eptr]() {
    try {
      door.unlock();
    } catch (...) {
      eptr = std::current_exception();
    }
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  door.lock();
  worker.join();
  if (eptr) {
    std::rethrow_exception(eptr);
  }
}
