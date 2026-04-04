// Copyright 2021 GHA Test Team

#include "TimedDoor.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <stdexcept>
#include <thread>

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

void CloseIfOpen(Door& d) {
  if (d.isDoorOpened()) {
    d.lock();
  }
}

}  // namespace

class TimedDoorTest : public ::testing::Test {
 protected:
  void SetUp() override { door_ = std::make_unique<TimedDoor>(0); }

  void TearDown() override { door_.reset(); }

  std::unique_ptr<TimedDoor> door_;
};

class TimerTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}

  Timer timer_;
};

class MockDoorInteractionTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(TimedDoorTest, get_timeout_matches_constructor) {
  EXPECT_EQ(door_->getTimeOut(), 0);
  TimedDoor other(42);
  EXPECT_EQ(other.getTimeOut(), 42);
}

TEST_F(TimedDoorTest, starts_closed) {
  EXPECT_FALSE(door_->isDoorOpened());
}

TEST_F(TimedDoorTest, lock_keeps_door_closed) {
  door_->lock();
  EXPECT_FALSE(door_->isDoorOpened());
}

TEST_F(TimedDoorTest, unlock_with_zero_timeout_throws_runtime_error) {
  EXPECT_THROW(door_->unlock(), std::runtime_error);
}

TEST_F(TimedDoorTest, throw_state_when_closed_does_not_throw) {
  door_->lock();
  EXPECT_NO_THROW(door_->throwState());
}

TEST_F(TimedDoorTest, second_unlock_throws_when_door_still_open) {
  TimedDoor d(0);
  EXPECT_THROW(d.unlock(), std::runtime_error);
  EXPECT_THROW(d.unlock(), std::runtime_error);
}

TEST_F(TimedDoorTest, is_door_opened_true_while_timer_pending) {
  TimedDoor door(200);
  door.lock();
  std::thread worker([&door]() {
    try {
      door.unlock();
    } catch (const std::runtime_error&) {
      // Door may alarm if still open at timeout; we only poll isDoorOpened().
    }
  });
  bool saw_open = false;
  for (int i = 0; i < 500 && !saw_open; ++i) {
    if (door.isDoorOpened()) {
      saw_open = true;
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  EXPECT_TRUE(saw_open);
  worker.join();
}

TEST_F(TimerTest, tregister_zero_delay_invokes_timeout) {
  testing::NiceMock<MockTimerClient> mock;
  EXPECT_CALL(mock, Timeout()).Times(1);
  timer_.tregister(0, &mock);
}

TEST_F(TimerTest, tregister_short_delay_invokes_timeout) {
  testing::NiceMock<MockTimerClient> mock;
  EXPECT_CALL(mock, Timeout()).Times(1);
  timer_.tregister(2, &mock);
}

TEST_F(MockDoorInteractionTest, close_if_open_calls_lock_when_opened) {
  testing::StrictMock<MockDoor> door;
  EXPECT_CALL(door, isDoorOpened()).WillOnce(testing::Return(true));
  EXPECT_CALL(door, lock()).Times(1);
  CloseIfOpen(door);
}

TEST_F(MockDoorInteractionTest, close_if_open_skips_lock_when_closed) {
  testing::StrictMock<MockDoor> door;
  EXPECT_CALL(door, isDoorOpened()).WillOnce(testing::Return(false));
  EXPECT_CALL(door, lock()).Times(0);
  CloseIfOpen(door);
}

TEST(TimedDoorConcurrency, lock_before_timeout_avoids_exception) {
  TimedDoor door(120);
  door.lock();
  std::exception_ptr error;
  std::thread worker([&door, &error]() {
    try {
      door.unlock();
    } catch (...) {
      error = std::current_exception();
    }
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(25));
  door.lock();
  worker.join();
  EXPECT_EQ(error, nullptr);
}
