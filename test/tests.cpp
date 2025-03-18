// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"

using ::testing::_;

class MockTimedDoor : public TimedDoor {
public:
    MockTimedDoor(int timeout) : TimedDoor(timeout) {}
    MOCK_METHOD(void, throwState, (), (override));
};

class MockTimerClient : public TimerClient {
public:
    MOCK_METHOD(void, Timeout, (), (override));
};

TEST(TimedDoorTest, DoorLockUnlock) {
    TimedDoor door(5);
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

TEST(TimedDoorTest, DoorTimeoutExcep) {
    TimedDoor door(1);
    door.unlock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST(TimedDoorTest, DoorNoTimeoutException) {
    TimedDoor door(1);
    door.lock();
    EXPECT_NO_THROW(door.throwState());
}

TEST(DoorTimerAdapterTest, TimeoutCalled) {
    MockTimedDoor door(1);
    DoorTimerAdapter adapter(door);

    EXPECT_CALL(door, throwState()).Times(1);
    adapter.Timeout();
}

TEST(TimerTest, TimerRegistersClient) {
    MockTimerClient client;
    Timer timer;

    EXPECT_CALL(client, Timeout()).Times(1);
    timer.tregister(1, &client);
}

TEST(TimedDoorTest, DoorUnlockTriggersTimeout) {
    MockTimedDoor door(1);
    EXPECT_CALL(door, throwState()).Times(1);
    door.unlock();
}

TEST(TimedDoorTest, DoorLockPreventsTimeout) {
    MockTimedDoor door(1);
    EXPECT_CALL(door, throwState()).Times(0);
    door.lock();
}

TEST(TimedDoorTest, DoorTimeoutValue) {
    TimedDoor door(10);
    EXPECT_EQ(door.getTimeOut(), 10);
}

TEST(TimedDoorTest, DoorInitialState) {
    TimedDoor door(5);
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, DoorUnlockState) {
    TimedDoor door(5);
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}
