// Copyright 2025 Yaroslav

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <chrono> // NOLINT [build/c++11]
#include <thread> // NOLINT [build/c++11]

#include "TimedDoor.h"


class MockTimer : public Timer {
 public:
    MOCK_METHOD(void, tregister, (int, TimerClient*));
};

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
    TimedDoor door;
    MockTimerClient* mockClient;
    MockTimer mockTimer;
 public:
    TimedDoorTest() : door(10), mockTimer() {}
 protected:
    void SetUp() override {
        mockClient = new MockTimerClient();
    }
    void TearDown() override {
        delete mockClient;
    }
};

TEST_F(TimedDoorTest, InitialState) {
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockDoor) {
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, ExceptionThrownOnUnlockIfAlreadyClosed) {
    ASSERT_NO_THROW(door.unlock());
}

TEST_F(TimedDoorTest, LockDoor) {
    door.unlock();
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, LockAfterUnlockClosesDoor) {
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockAlreadyOpenedDoor) {
    door.unlock();
    EXPECT_THROW(door.unlock(), std::logic_error);
}

TEST_F(TimedDoorTest, LockAlreadyOpenedDoor) {
    EXPECT_THROW(door.lock(), std::logic_error);
}

TEST_F(TimedDoorTest, ExceptionThrownOnTimeoutWithClosedDoor) {
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(11));
    ASSERT_NO_THROW(door.lock());
}

TEST_F(TimedDoorTest, DoorRemainsOpenAfterTimeout) {
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(11));
    ASSERT_TRUE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, ExceptionThrownState) {
    EXPECT_NO_THROW(door.throwState());
}
