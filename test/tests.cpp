// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::Throw;

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    door = new TimedDoor(5);
  }

  void TearDown() override {
    delete door;
  }

  TimedDoor* door;
};

TEST_F(TimedDoorTest, InitialStateIsClosed) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
    door->unlock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutThrowsWhenDoorOpen) {
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

TEST(TimerTest, TregisterCallsTimeoutAfterSleep) {
    MockTimerClient client;
    Timer timer;
    EXPECT_CALL(client, Timeout()).Times(1);
    timer.tregister(1, &client);
}

TEST(DoorTimerAdapterTest, TimeoutThrowsIfDoorOpen) {
    TimedDoor door(5);
    door.unlock();
    DoorTimerAdapter adapter(door);
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, TimeoutNoThrowIfDoorClosed) {
    TimedDoor door(5);
    door.lock();
    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST(TimedDoorTest, UnlockRegistersTimer) {
    TimedDoor door(1);
    door.unlock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
  }

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
