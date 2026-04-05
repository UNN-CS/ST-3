// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <stdexcept>
#include <chrono>
#include <memory>
#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
    std::unique_ptr<TimedDoor> door;
    void SetUp() override {
        door = std::make_unique<TimedDoor>(100);
    }
    void TearDown() override {
        door.reset();
    }
};

TEST_F(TimedDoorTest, init_closed) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, unlock_opened) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, lock_closed) {
    door->unlock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, getTimeOut_100) {
    EXPECT_EQ(100, door->getTimeOut());
}

TEST_F(TimedDoorTest, throwState_closed) {
    EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, throwState_opened) {
    door->unlock();
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, Timeout_opened) {
    TimedDoor door(100);
    door.unlock();
    DoorTimerAdapter adapter(door);
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, Timeout_closed) {
    TimedDoor door(100);
    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST(TimerTest, tregister_Timeout) {
    Timer timer;
    MockTimerClient mockClient;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    timer.tregister(100, &mockClient);
}

TEST(TimerTest, tregister_sleep) {
    Timer timer;
    MockTimerClient mockClient;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    auto start_time = std::chrono::steady_clock::now();
    timer.tregister(100, &mockClient);
    auto end_time = std::chrono::steady_clock::now();
    auto sleep_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time - start_time).count();
    EXPECT_GE(sleep_time, 100);
}

TEST(FridgeTest, tregister_exception_opened) {
    TimedDoor door(100);
    door.unlock();
    DoorTimerAdapter adapter(door);
    Timer timer;
    EXPECT_THROW(timer.tregister(100, &adapter), std::runtime_error);
}

TEST(FridgeTest, tregister_exception_closed) {
    TimedDoor door(100);
    DoorTimerAdapter adapter(door);
    Timer timer;
    EXPECT_NO_THROW(timer.tregister(100, &adapter));
}
