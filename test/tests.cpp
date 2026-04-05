// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <chrono>
#include <memory>
#include <stdexcept>
#include <thread>

#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door {
 public:
  MOCK_METHOD(void, lock, (), (override));
  MOCK_METHOD(void, unlock, (), (override));
  MOCK_METHOD(bool, isDoorOpened, (), (const, override));
};

class DoorOperator {
 public:
  explicit DoorOperator(Door& door) : door_(door) {}

  void closeDoor() { door_.lock(); }

  void openDoor() { door_.unlock(); }

  bool isOpen() const { return door_.isDoorOpened(); }

 private:
  Door& door_;
};

class NoopTimer : public Timer {
 public:
  void tregister(int, TimerClient*) override {}
};

class InstantSleepTimer : public Timer {
 protected:
  void sleep(int) override {}
};

class TimedDoorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    door_ = std::make_unique<TimedDoor>(5);
  }

  void TearDown() override { door_.reset(); }

  std::unique_ptr<TimedDoor> door_;
};

TEST_F(TimedDoorTest, GetTimeOutReturnsConfiguredValue) {
  EXPECT_EQ(door_->getTimeOut(), 5);
}

TEST_F(TimedDoorTest, DoorStartsClosed) {
  EXPECT_FALSE(door_->isDoorOpened());
}

TEST_F(TimedDoorTest, LockKeepsDoorClosed) {
  ASSERT_FALSE(door_->isDoorOpened());
  door_->lock();
  EXPECT_FALSE(door_->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockWithZeroDelayThrowsIfLeftOpen) {
  TimedDoor door(0);
  EXPECT_THROW(door.unlock(), std::runtime_error);
}

TEST_F(TimedDoorTest, ThrowStateDoesNotThrowWhenClosed) {
  door_->lock();
  EXPECT_NO_THROW(door_->throwState());
}

TEST_F(TimedDoorTest, ThrowStateThrowsWhenOpenWithNoopTimer) {
  NoopTimer noop;
  door_->setTimerForTest(&noop);
  door_->unlock();
  EXPECT_THROW(door_->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, UnlockWithNoopTimerLeavesDoorOpen) {
  NoopTimer noop;
  door_->setTimerForTest(&noop);
  door_->unlock();
  EXPECT_TRUE(door_->isDoorOpened());
}

TEST(TimerTest, TRegisterInvokesTimeoutOnClient) {
  MockTimerClient client;
  EXPECT_CALL(client, Timeout()).Times(1);
  Timer timer;
  timer.tregister(0, &client);
}

TEST(TimerTest, TRegisterWithNullClientDoesNotCrash) {
  Timer timer;
  EXPECT_NO_THROW(timer.tregister(0, nullptr));
}

TEST(MockDoorTest, CloseDoorCallsLockOnInterface) {
  MockDoor mock;
  DoorOperator op(mock);
  EXPECT_CALL(mock, lock()).Times(1);
  op.closeDoor();
}

TEST(MockDoorTest, OpenDoorCallsUnlockOnInterface) {
  MockDoor mock;
  DoorOperator op(mock);
  EXPECT_CALL(mock, unlock()).Times(1);
  op.openDoor();
}

TEST(MockDoorTest, IsOpenQueriesDoor) {
  MockDoor mock;
  DoorOperator op(mock);
  EXPECT_CALL(mock, isDoorOpened())
      .Times(1)
      .WillOnce(::testing::Return(true));
  EXPECT_TRUE(op.isOpen());
}

TEST(TimedDoorConcurrencyTest, LockDuringCountdownAvoidsException) {
  TimedDoor door(1);
  std::thread worker([&door]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    door.lock();
  });
  EXPECT_NO_THROW(door.unlock());
  worker.join();
}

TEST(DoorTimerAdapterTest, TimeoutPropagatesThroughTimedDoor) {
  TimedDoor door(0);
  InstantSleepTimer fast;
  door.setTimerForTest(&fast);
  EXPECT_THROW(door.unlock(), std::runtime_error);
}
