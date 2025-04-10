// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <thread>
#include <memory>
#include "TimedDoor.h"

using ::testing::AtLeast;
using ::testing::Mock;
using ::testing::_;

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

TEST_F(TimedDoorTest, initially_closed) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, door_opens) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, door_close) {
    door->unlock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, time_out_throws_exception_when_door_open) {
    door->unlock();
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, time_out_no_throws_exception_when_door_closed) {
    door->lock();
    EXPECT_NO_THROW(door->throwState());
}


TEST_F(TimedDoorTest, adapter_timeout) {
    door->unlock();
    std::this_thread::sleep_for(std::chrono::seconds(6));
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, get_timeout) {
    EXPECT_EQ(door->getTimeOut(), 5);
}

TEST_F(TimedDoorTest, locking_twice_door_lock) {
    door->lock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, unlocking_twice_door_unlock) {
    door->unlock();
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, no_timeout_when_door_locked) {
    door->unlock();
    door->lock();
    EXPECT_NO_THROW(adapter->Timeout());
}
