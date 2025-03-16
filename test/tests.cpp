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

TEST(TimedDoorTest, ThrowStateWhenOpenThrows) {
    TimedDoor door(5);
    door.unlock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST(TimedDoorTest, ThrowStateWhenClosedNoThrow) {
    TimedDoor door(5);
    EXPECT_NO_THROW(door.throwState());
}

TEST(DoorTimerAdapterTest, TimeoutThrowsWhenDoorOpen) {
    TimedDoor door(5);
    door.unlock();
    DoorTimerAdapter adapter(door);
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, TimeoutNoThrowWhenDoorClosed) {
    TimedDoor door(5);
    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST(TimedDoorTest, GetTimeoutReturnsCorrectValue) {
    TimedDoor door(10);
    EXPECT_EQ(door.getTimeOut(), 10);
}

TEST(TimerTest, TregisterCallsTimeout) {
    MockTimerClient client;
    EXPECT_CALL(client, Timeout()).Times(1);
    Timer timer;
    timer.tregister(0, &client);
}

TEST(TimedDoorIntegrationTest, CloseDoorBeforeTimeout) {
    TimedDoor door(1);
    door.unlock();
    door.lock();
    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST(TimedDoorIntegrationTest, DoorOpenAfterTimeoutThrows) {
    TimedDoor door(1);
    door.unlock();
    DoorTimerAdapter adapter(door);
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}