// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <thread>
#include "../include/TimedDoor.h"

using ::testing::_;
using ::testing::Invoke;

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
    TimedDoor* door;
    DoorTimerAdapter* adapter;
    Timer* timer;
    void SetUp() override {
        door = new TimedDoor(5);
        adapter = new DoorTimerAdapter(*door);
        timer = new Timer();
    }
    void TearDown() override {
        delete door;
        delete adapter;
        delete timer;
    }
};

TEST_F(TimedDoorTest, DoorInitialStateIsClosed) {
    ASSERT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorUnlocksCorrectly) {
    door->unlock();
    ASSERT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorLocksCorrectly) {
    door->unlock();
    door->lock();
    ASSERT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutThrowsExceptionWhenDoorIsOpen) {
    door->unlock();
    EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, TimeoutDoesNotThrowExceptionWhenDoorIsClosed) {
    door->lock();
    EXPECT_NO_THROW(adapter->Timeout());
}

TEST_F(TimedDoorTest, TimerRegistersTimeoutCorrectly) {
    MockTimerClient mockClient;
    Timer timer;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    timer.tregister(1, &mockClient);
}

TEST_F(TimedDoorTest, DoorTimeoutValueIsCorrect) {
    ASSERT_EQ(door->getTimeOut(), 5);
}

TEST_F(TimedDoorTest, TimerAdapterHandlesMultipleTimeouts) {
    door->unlock();
    EXPECT_THROW(adapter->Timeout(), std::runtime_error);

    door->lock();
    EXPECT_NO_THROW(adapter->Timeout());
}
TEST_F(TimedDoorTest, UnlockingDoorTwiceKeepsItOpen) {
    door->unlock();
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockingDoorTwiceDoesNotAffectState) {
    door->lock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

