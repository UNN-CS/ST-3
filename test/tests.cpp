// Copyright 2025

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <chrono> // NOLINT [build/c++11]
#include <thread> // NOLINT [build/c++11]
#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
    void SetUp() override {
        timerClient = new MockTimerClient();
    }

    void TearDown() override {
        delete timerClient;
    }

    TimedDoor door;
    Timer timer;
    MockTimerClient* timerClient{};

 public:
    TimedDoorTest() : door(5), timer() {}
};

TEST_F(TimedDoorTest, IsDoorClosed) {
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, IsDoorOpened) {
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, DoorLockedNow) {
    EXPECT_ANY_THROW(door.lock());
}

TEST_F(TimedDoorTest, DoorUnlockedNow) {
    door.unlock();
    EXPECT_ANY_THROW(door.unlock());
}

TEST_F(TimedDoorTest, LockingForClosed) {
    timer.tregister(door.getTimeOut(), timerClient);
    EXPECT_ANY_THROW(door.lock());
}

TEST_F(TimedDoorTest, DoorOpenedAndClosed) {
    door.unlock();
    EXPECT_NO_THROW(door.lock());
}

TEST_F(TimedDoorTest, IsDoorOpenedAndClosed) {
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, CheckLockingDoorForNoTimeOut) {
    door.unlock();
    timer.tregister(door.getTimeOut(), timerClient);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, LockingDoorForIsTimeOut) {
    door.unlock();
    timer.tregister(door.getTimeOut(), timerClient);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    EXPECT_ANY_THROW(door.throwState());
}

TEST_F(TimedDoorTest, LockingDoorForNoTimeOut) {
    door.unlock();
    timer.tregister(door.getTimeOut(), timerClient);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    EXPECT_NO_THROW(door.lock());
}
