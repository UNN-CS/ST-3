#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <stdexcept>

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
    door_ = std::make_unique<TimedDoor>(0); // ВАЖНО: timeout = 0
    door_->lock();
  }

  void TearDown() override {
    door_.reset();
  }

  std::unique_ptr<TimedDoor> door_;
};

TEST_F(TimedDoorTest, InitiallyClosed) {
  EXPECT_FALSE(door_->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoorDespiteException) {
  try {
    door_->unlock();
  } catch (...) {
  }
  EXPECT_TRUE(door_->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
  try {
    door_->unlock();
  } catch (...) {}

  door_->lock();
  EXPECT_FALSE(door_->isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutValueCorrect) {
  EXPECT_EQ(door_->getTimeOut(), 0);
}

TEST_F(TimedDoorTest, UnlockThrowsException) {
  EXPECT_THROW(door_->unlock(), std::runtime_error);
}

TEST_F(TimedDoorTest, NoExceptionIfDoorClosed) {
  door_->lock();
  EXPECT_NO_THROW(door_->throwState());
}

TEST(DoorTimerAdapterTest, AdapterCallsThrowState) {
  TimedDoor door(0);

  try {
    door.unlock();
  } catch (...) {}

  DoorTimerAdapter adapter(door);

  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(TimerTest, TimerCallsClientTimeout) {
  Timer timer;
  MockTimerClient mock;

  EXPECT_CALL(mock, Timeout()).Times(1);

  timer.tregister(0, &mock);
}

TEST(TimerTest, TimerWithNullClientDoesNothing) {
  Timer timer;

  EXPECT_NO_THROW(timer.tregister(0, nullptr));
}

TEST(MockTest, MockDoorUsage) {
  MockDoor door;

  EXPECT_CALL(door, isDoorOpened()).WillOnce(Return(true));

  EXPECT_TRUE(door.isDoorOpened());
}

TEST(DoorTimerAdapterTest, AdapterDoesNotThrowIfDoorClosed) {
  TimedDoor door(0);
  door.lock();

  DoorTimerAdapter adapter(door);

  EXPECT_NO_THROW(adapter.Timeout());
}

TEST(TimedDoorTestStandalone, UnlockAlwaysThrowsWithZeroTimeout) {
  TimedDoor door(0);
  EXPECT_THROW(door.unlock(), std::runtime_error);
}
