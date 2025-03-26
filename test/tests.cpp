// Copyright 2021 GHA Test Team
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"
#include <cstdint>
#include <chrono>
#include <thread>

class TimedDoorFixture : public ::testing::Test {
 protected:
    TimedDoor* door;
    void SetUp() override {
        door = new TimedDoor(1);
    }
    void TearDown() override {
        try {
            door->joinTimerThread();
        }
        catch (...) {
        }
        delete door;
    }
};

TEST_F(TimedDoorFixture, DoorIsClosedAfterLock) {
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorFixture, DoorIsOpenedAfterUnlock) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    EXPECT_THROW(door->joinTimerThread(), std::runtime_error);
}

TEST_F(TimedDoorFixture, ExceptionThrownWhenDoorRemainsOpen) {
    door->unlock();
    EXPECT_THROW(door->joinTimerThread(), std::runtime_error);
}

TEST_F(TimedDoorFixture, NoExceptionWhenDoorClosedBeforeTimeout) {
    TimedDoor doorLocal(2);
    doorLocal.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    doorLocal.lock();
    EXPECT_NO_THROW(doorLocal.joinTimerThread());
}

TEST_F(TimedDoorFixture, GetTimeOutReturnsCorrectValue) {
    int timeout = 3;
    TimedDoor doorLocal(timeout);
    EXPECT_EQ(doorLocal.getTimeOut(), timeout);
}

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

TEST(TimedDoorTests, AdapterTimeoutIsCalled) {
    MockTimerClient mockClient;
    EXPECT_CALL(mockClient, Timeout()).Times(1);

    Timer timer;
    timer.tregister(0, &mockClient);
}

TEST_F(TimedDoorFixture, LockClosesDoor) {
    door->unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorFixture, UnlockOpensDoor) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    EXPECT_THROW(door->joinTimerThread(), std::runtime_error);
}

TEST_F(TimedDoorFixture, JoinTimerThreadWorksCorrectly) {
    door->unlock();
    EXPECT_THROW(door->joinTimerThread(), std::runtime_error);
}

TEST_F(TimedDoorFixture, MultipleJoinTimerThreadCallsDoNotFail) {
    door->unlock();
    try {
        door->joinTimerThread();
    }
    catch (...) {
    }

    EXPECT_NO_THROW(door->joinTimerThread());
}
