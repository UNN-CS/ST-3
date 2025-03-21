// Copyright 2021 GHA Test Team
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include "TimedDoor.h"

using ::testing::_;
using ::testing::Mock;

class MockTimer : public Timer {
 public:
  MOCK_METHOD(void, tregister, (int, TimerClient*), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
  MockTimer mockTimer;
  std::unique_ptr<TimedDoor> door;

  void SetUp() override {
    door = std::make_unique<TimedDoor>(5, &mockTimer);
  }
};

TEST_F(TimedDoorTest, InitialStateIsClosed) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
    EXPECT_CALL(mockTimer, tregister(5, _));
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
    door->unlock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutThrowsWhenDoorOpen) {
    EXPECT_CALL(mockTimer, tregister(_, _));
    door->unlock();
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, NoThrowWhenDoorClosed) {
    door->lock();
    EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, GetTimeoutReturnsCorrectValue) {
    EXPECT_EQ(door->getTimeOut(), 5);
}

TEST(DoorTimerAdapterTest, TimeoutThrowsIfDoorOpen) {
    MockTimer timer;
    TimedDoor door(5, &timer);
    door.unlock();
    DoorTimerAdapter adapter(&door);
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, TimeoutNoThrowIfDoorClosed) {
    MockTimer timer;
    TimedDoor door(5, &timer);
    door.lock();
    DoorTimerAdapter adapter(&door);
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST_F(TimedDoorTest, UnlockRegistersTimer) {
    EXPECT_CALL(mockTimer, tregister(5, _));
    door->unlock();
}
