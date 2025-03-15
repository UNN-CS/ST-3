// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Return;

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

TEST(TimedDoorTest, DoorLockUnlockTest) {
    TimedDoor door(5);
    EXPECT_FALSE(door.isDoorOpened());
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, TimeoutTest) {
    TimedDoor door(1);
    door.unlock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST(TimedDoorTest, NoTimeoutWhenLocked) {
    TimedDoor door(1);
    door.unlock();
    door.lock();
    EXPECT_NO_THROW(door.throwState());
}

TEST(DoorTimerAdapterTest, TimeoutCalled) {
    TimedDoor door(1);
    DoorTimerAdapter adapter(door);
    door.unlock();
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(TimerTest, RegisterTest) {
    MockTimerClient mockClient;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    Timer timer;
    timer.tregister(1, &mockClient);
}

TEST(TimedDoorTest, AdapterTimeout) {
    TimedDoor door(1);
    DoorTimerAdapter adapter(door);
    door.unlock();
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(TimedDoorTest, AdapterNoTimeoutWhenLocked) {
    TimedDoor door(1);
    DoorTimerAdapter adapter(door);
    door.unlock();
    door.lock();
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST(TimedDoorTest, GetTimeout) {
    TimedDoor door(5);
    EXPECT_EQ(door.getTimeOut(), 5);
}

TEST(TimedDoorTest, ThrowStateWhenOpened) {
    TimedDoor door(1);
    door.unlock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST(TimedDoorTest, NoThrowStateWhenClosed) {
    TimedDoor door(1);
    door.unlock();
    door.lock();
    EXPECT_NO_THROW(door.throwState());
}

TEST(TimedDoorTest, TimerRegistration) {
    TimedDoor door(1);
    MockTimerClient mockClient;
    Timer timer;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    timer.tregister(1, &mockClient);
}
