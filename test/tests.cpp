// Copyright 2021 GHA Test Team

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <future>
#include <memory>
#include <stdexcept>
#include <thread>

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

void ensureDoorClosed(Door* door) {
  if (door->isDoorOpened()) {
    door->lock();
  }
}

void openDoor(Door* door) {
  door->unlock();
}

class TimedDoorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    door = std::make_unique<TimedDoor>(10);
    timer = std::make_unique<Timer>();
    adapter = std::make_unique<DoorTimerAdapter>(*door);
  }

  void TearDown() override {
    adapter.reset();
    timer.reset();
    door.reset();
  }

  std::unique_ptr<TimedDoor> door;
  std::unique_ptr<Timer> timer;
  std::unique_ptr<DoorTimerAdapter> adapter;
};

}  // namespace

TEST_F(TimedDoorTest, constructor_sets_timeout) {
  EXPECT_EQ(10, door->getTimeOut());
}

TEST_F(TimedDoorTest, constructor_creates_closed_door) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, lock_keeps_door_closed) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, throwState_does_not_throw_for_closed_door) {
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, adapter_timeout_does_not_throw_for_closed_door) {
  door->lock();
  EXPECT_NO_THROW(adapter->Timeout());
}

TEST_F(TimedDoorTest, timer_register_calls_timeout_once) {
  MockTimerClient client;

  EXPECT_CALL(client, Timeout()).Times(1);
  EXPECT_NO_THROW(timer->tregister(1, &client));
}

TEST(TimedDoorMockTest, ensureDoorClosed_calls_lock_for_opened_door) {
  MockDoor door;

  EXPECT_CALL(door, isDoorOpened()).Times(1).WillOnce(Return(true));
  EXPECT_CALL(door, lock()).Times(1);

  ensureDoorClosed(&door);
}

TEST(TimedDoorMockTest, ensureDoorClosed_does_not_call_lock_for_closed_door) {
  MockDoor door;

  EXPECT_CALL(door, isDoorOpened()).Times(1).WillOnce(Return(false));
  EXPECT_CALL(door, lock()).Times(0);

  ensureDoorClosed(&door);
}

TEST(TimedDoorMockTest, openDoor_calls_unlock_once) {
  MockDoor door;

  EXPECT_CALL(door, unlock()).Times(1);
  openDoor(&door);
}

TEST(TimedDoorBehaviourTest, unlock_throws_if_door_remains_open_until_timeout) {
  TimedDoor door(5);

  EXPECT_THROW(door.unlock(), std::runtime_error);
  EXPECT_TRUE(door.isDoorOpened());
}

TEST(TimedDoorBehaviourTest,
     unlock_does_not_throw_if_door_is_closed_before_timeout) {
  TimedDoor door(30);

  auto result = std::async(std::launch::async, [&door]() { door.unlock(); });

  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  door.lock();

  EXPECT_NO_THROW(result.get());
  EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorBehaviourTest,
     timer_with_adapter_throws_for_opened_door_after_timeout) {
  TimedDoor door(40);
  DoorTimerAdapter adapter(door);
  Timer timer;

  auto opener = std::async(std::launch::async, [&door]() {
    try {
      door.unlock();
    } catch (const std::runtime_error&) {
    }
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  EXPECT_THROW(timer.tregister(1, &adapter), std::runtime_error);
  opener.wait();
}

TEST(TimedDoorBehaviourTest,
     timer_with_adapter_does_not_throw_for_closed_door_after_timeout) {
  TimedDoor door(5);
  DoorTimerAdapter adapter(door);
  Timer timer;

  door.lock();
  EXPECT_NO_THROW(timer.tregister(1, &adapter));
}

TEST(TimedDoorBehaviourTest, unlock_can_be_observed_as_open_before_timeout) {
  TimedDoor door(30);

  auto result = std::async(std::launch::async, [&door]() {
    try {
      door.unlock();
    } catch (const std::runtime_error&) {
    }
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  EXPECT_TRUE(door.isDoorOpened());

  door.lock();
  result.wait();
}
