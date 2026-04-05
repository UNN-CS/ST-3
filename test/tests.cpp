// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <atomic>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <string>
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
    door_ = std::make_unique<TimedDoor>(120);
  }

  void TearDown() override {
    door_.reset();
  }

  std::unique_ptr<TimedDoor> door_;
};

TEST_F(TimedDoorTest, DoorInitiallyClosed) {
  EXPECT_FALSE(door_->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoorWhileTimerRuns) {
  std::atomic<bool> saw_open{false};
  std::thread worker([&]() {
    try {
      door_->unlock();
    } catch (const std::runtime_error&) {
    }
  });
  const auto deadline = std::chrono::steady_clock::now() +
                        std::chrono::milliseconds(100);
  while (std::chrono::steady_clock::now() < deadline && !saw_open) {
    if (door_->isDoorOpened()) {
      saw_open = true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  EXPECT_TRUE(saw_open);
  worker.join();
}

TEST_F(TimedDoorTest, LockClosesDoorBeforeTimeoutEnds) {
  std::atomic<bool> unlock_threw{false};
  std::thread worker([&]() {
    try {
      door_->unlock();
    } catch (...) {
      unlock_threw = true;
    }
  });
  while (!door_->isDoorOpened()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  door_->lock();
  worker.join();
  EXPECT_FALSE(unlock_threw);
  EXPECT_FALSE(door_->isDoorOpened());
}

TEST(TimedDoorConfigTest, GetTimeOutReturnsConfiguredValue) {
  TimedDoor door(42);
  EXPECT_EQ(door.getTimeOut(), 42);
}

TEST(TimerTest, TRegisterInvokesClientTimeout) {
  MockTimerClient mock;
  EXPECT_CALL(mock, Timeout()).Times(1);
  Timer timer;
  timer.tregister(0, &mock);
}

TEST(TimerTest, TRegisterWithNullClientDoesNotCrash) {
  Timer timer;
  ASSERT_NO_THROW(timer.tregister(0, nullptr));
}

TEST(MockDoorTest, LockUnlockAndIsDoorOpenedAreCallable) {
  MockDoor mock;
  EXPECT_CALL(mock, lock()).Times(1);
  EXPECT_CALL(mock, unlock()).Times(1);
  EXPECT_CALL(mock, isDoorOpened()).WillOnce(Return(true));
  mock.lock();
  mock.unlock();
  EXPECT_TRUE(mock.isDoorOpened());
}

TEST(TimedDoorExceptionTest, OpenDoorAtTimeoutThrowsRuntimeError) {
  TimedDoor door(0);
  EXPECT_THROW(door.unlock(), std::runtime_error);
}

TEST(TimedDoorExceptionTest, UnlockThrowsRuntimeErrorWithMessage) {
  TimedDoor door(0);
  try {
    door.unlock();
    FAIL() << "Expected std::runtime_error";
  } catch (const std::runtime_error& e) {
    EXPECT_FALSE(std::string(e.what()).empty());
  }
}

TEST(TimedDoorExceptionTest, ClosedDoorAtTimeoutDoesNotThrow) {
  TimedDoor door(80);
  std::atomic<bool> unlocking_started{false};

  std::thread worker([&]() {
    unlocking_started = true;
    ASSERT_NO_THROW(door.unlock());
  });

  while (!unlocking_started) {
    std::this_thread::yield();
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(15));
  door.lock();

  worker.join();
}

TEST(TimedDoorExceptionTest, AfterExceptionDoorCanBeLockedAgain) {
  TimedDoor door(0);
  EXPECT_THROW(door.unlock(), std::runtime_error);
  EXPECT_TRUE(door.isDoorOpened());
  door.lock();
  EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, LockWhileAlreadyClosedKeepsDoorClosed) {
  door_->lock();
  EXPECT_FALSE(door_->isDoorOpened());
}
