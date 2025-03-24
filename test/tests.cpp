// test/tests.cpp
// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"
#include <thread>
#include <chrono>
#include <stdexcept>

using ::testing::AtLeast;

class TimedDoorTest : public ::testing::Test {
protected:
    TimedDoor* door;
    void SetUp() override {
        door = new TimedDoor(1);
    }
    void TearDown() override {
        delete door;
    }
};

class MockTimerClient : public TimerClient {
public:
    MOCK_METHOD(void, Timeout, (), (override));
};

TEST_F(TimedDoorTest, DoorInitialStateIsClosed) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockSetsDoorToOpen) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());

    std::this_thread::sleep_for(std::chrono::seconds(2));
}

TEST_F(TimedDoorTest, LockSetsDoorToClosed) {
    door->unlock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

TEST_F(TimedDoorTest, GetTimeOutReturnsCorrectValue) {
    EXPECT_EQ(door->getTimeOut(), 1);
}

TEST_F(TimedDoorTest, ExceptionThrownWhenDoorRemainsOpenAfterTimeout) {
    EXPECT_THROW({
        door->unlock();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }, std::runtime_error);
}

TEST_F(TimedDoorTest, NoExceptionWhenDoorClosedBeforeTimeout) {
    EXPECT_NO_THROW({
        door->unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        door->lock();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    });
}

TEST(TimerAdapterTest, TimeoutCallsThrowStateIfDoorOpen) {
    TimedDoor door(0);
    door.unlock();
    EXPECT_THROW({
        door.throwState();
    }, std::runtime_error);
}

TEST(TimerTest, TimerRegistersAndCallsTimeout) {
    MockTimerClient mockClient;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    Timer timer;
    timer.tregister(1, &mockClient);
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

TEST(TimerTest, TimerWithZeroTimeoutCallsImmediately) {
    MockTimerClient mockClient;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    Timer timer;
    timer.tregister(0, &mockClient);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST_F(TimedDoorTest, DoorThrowStateThrowsRuntimeError) {
    EXPECT_THROW(door->throwState(), std::runtime_error);
}
