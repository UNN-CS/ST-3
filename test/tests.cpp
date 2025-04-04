// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"
#include <thread>

using ::testing::_;
using ::testing::Mock;
using ::testing::InSequence;

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

TEST(TimedDoorTest, InitialStateIsClosed) {
    TimedDoor door(5);
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, UnlockOpensDoor) {
    TimedDoor door(5);
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

TEST(TimedDoorTest, LockClosesDoor) {
    TimedDoor door(5);
    door.unlock();
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, TimeoutWhenOpenThrowsException) {
    TimedDoor door(5);
    door.unlock();
    DoorTimerAdapter adapter(door);
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(TimedDoorTest, TimeoutWhenClosedDoesNotThrow) {
    TimedDoor door(5);
    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST(TimedDoorTest, ThrowStateThrowsWhenOpen) {
    TimedDoor door(5);
    door.unlock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST(TimedDoorTest, ThrowStateDoesNotThrowWhenClosed) {
    TimedDoor door(5);
    EXPECT_NO_THROW(door.throwState());
}

TEST(TimedDoorTest, GetTimeoutReturnsCorrectValue) {
    TimedDoor door(10);
    EXPECT_EQ(door.getTimeOut(), 10);
}

TEST(TimedDoorTest, TimeoutAfterLockDoesNotThrow) {
    TimedDoor door(5);
    door.unlock();
    door.lock();
    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST(TimedDoorTest, TimerCallsTimeout) {
    MockTimerClient mockClient;
    Timer timer;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    try {
        timer.tregister(0, &mockClient);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    catch (const std::exception& e) {
        FAIL() << "Exception: " << e.what();
    }
}
