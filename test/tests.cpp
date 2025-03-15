// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono> 
#include <cstdint>
#include <thread>
#include "TimedDoor.h"

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
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTestFixture, NoTimeoutWhenLocked) {
    door->unlock();
    door->lock();
    EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTestFixture, AdapterTimeout) {
    door->unlock();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_THROW(adapter->Timeout(), std::runtime_error);
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

TEST(TimerTest, RegisterTest) {
    MockTimerClient mockClient;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    Timer timer;
    timer.tregister(1, &mockClient);
}

TEST(DoorTimerAdapterTest, TimeoutCalled) {
    TimedDoor door(1);
    DoorTimerAdapter adapter(door);
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}
