// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <memory>
#include <cstdint>
#include <thread>
#include "../include/TimedDoor.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Return;

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTestFixture : public ::testing::Test {
 protected:
    void SetUp() override {
        door = new TimedDoor(1);
        adapter = new DoorTimerAdapter(*door);
    }

    void TearDown() override {
        delete door;
        delete adapter;
    }

    TimedDoor* door;
    DoorTimerAdapter* adapter;
};

TEST_F(TimedDoorTestFixture, DoorLockUnlockTest) {
    EXPECT_FALSE(door->isDoorOpened());
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTestFixture, TimeoutTest) {
    door->unlock();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTestFixture, NoTimeoutWhenLocked) {
    door->unlock();
    door->lock();
    EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTestFixture, AdapterTimeout) {
    door->unlock();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTestFixture, AdapterNoTimeoutWhenLocked) {
    door->unlock();
    door->lock();
    EXPECT_NO_THROW(adapter->Timeout());
}

TEST_F(TimedDoorTestFixture, GetTimeout) {
    EXPECT_EQ(door->getTimeOut(), 1);
}

TEST_F(TimedDoorTestFixture, ThrowStateWhenOpened) {
    door->unlock();
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTestFixture, NoThrowStateWhenClosed) {
    door->unlock();
    door->lock();
    EXPECT_NO_THROW(door->throwState());
}

TEST(TimerTest, SimpleDelayTest) {
    Timer timer;
    auto start = std::chrono::steady_clock::now();

    timer.sleep(1000);

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>
        (end - start).count();

    EXPECT_GE(duration, 1000);
}

TEST(TimerTest, RegisterTest) {
    auto mockClient = std::make_shared<MockTimerClient>();
    EXPECT_CALL(*mockClient, Timeout()).Times(1);

    Timer timer;
    timer.tregister(1, mockClient.get());

    std::this_thread::sleep_for(std::chrono::seconds(2));
}

TEST(DoorTimerAdapterTest, TimeoutCalled) {
    TimedDoor door(1);
    DoorTimerAdapter adapter(door);
    door.unlock();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    EXPECT_NO_THROW(adapter.Timeout());
}
