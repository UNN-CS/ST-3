// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <thread>
#include <chrono>
#include "TimedDoor.h"

using ::testing::Return;

#define TIME 2

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

TEST(TimedDoorTest, DoorInitiallyClosed) {
    TimedDoor door(TIME);
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, DoorUnlocks) {
    TimedDoor door(TIME);
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

TEST(TimedDoorTest, DoorLocks) {
    TimedDoor door(TIME);
    door.unlock();
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, TimerTriggersTimeout) {
    MockTimerClient mockClient;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    Timer timer;
    timer.tregister(TIME, &mockClient);
    std::this_thread::sleep_for(std::chrono::seconds(TIME));
}

TEST(TimedDoorTest, ThrowsExceptionIfLeftOpen) {
    TimedDoor door(1);
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(TIME));
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST(TimedDoorTest, DoesNotThrowIfClosed) {
    TimedDoor door(1);
    door.unlock();
    door.lock();
    std::this_thread::sleep_for(std::chrono::seconds(TIME));
    EXPECT_NO_THROW(door.throwState());
}

TEST(TimedDoorTest, AdapterTriggersTimeout) {
    TimedDoor door(1);
    DoorTimerAdapter adapter(door);
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(TIME));
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(TimedDoorTest, TimerRegistersCorrectly) {
    MockTimerClient mockClient;
    Timer timer;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    timer.tregister(1, &mockClient);
}

TEST(TimedDoorTest, DoorRemainsLockedIfNotUnlocked) {
    TimedDoor door(TIME);
    EXPECT_FALSE(door.isDoorOpened());
    EXPECT_NO_THROW(door.throwState());
}

TEST(TimedDoorTest, TimerDoesNotTriggerWhenClosed) {
    MockDoor mockDoor;
    DoorTimerAdapter adapter((TimedDoor&)mockDoor);
    EXPECT_CALL(mockDoor, isDoorOpened()).WillOnce(Return(false));
    EXPECT_NO_THROW(adapter.Timeout());
}
