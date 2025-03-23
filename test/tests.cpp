// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <thread> // NOLINT [build/c++11]
#include <chrono> // NOLINT [build/c++11]

#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
    TimedDoor door;
    Timer timer;
    MockTimerClient *mockClient;
 public:
    TimedDoorTest() : door(1), timer() {}

 protected:
    void SetUp() override {
        mockClient = new MockTimerClient();
    }

    void TearDown() override {
        delete mockClient;
    }
};

TEST_F(TimedDoorTest, lockAfterUnlock) {
    door.unlock();
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, unlockAndLockMultipleTimes) {
    for (int i = 0; i < 3; ++i) {
        door.unlock();
        EXPECT_TRUE(door.isDoorOpened());
        door.lock();
        EXPECT_FALSE(door.isDoorOpened());
    }
}

TEST_F(TimedDoorTest, stateThrowOnUnlocked) {
    door.unlock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, stateThrowOnLocked) {
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, lockTwiceThrowsException) {
    door.unlock();
    door.lock();
    EXPECT_THROW(door.lock(), std::logic_error);
}

TEST_F(TimedDoorTest, unlockTwiceThrowsException) {
    door.unlock();
    EXPECT_THROW(door.unlock(), std::logic_error);
}

TEST_F(TimedDoorTest, doorTimeoutWhenOpened) {
    door.unlock();
    DoorTimerAdapter adapter(door);
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, doorTimeoutWhenClosed) {
    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST_F(TimedDoorTest, timerTimeoutTriggered) {
    EXPECT_CALL(*mockClient, Timeout()).Times(1);
    timer.tregister(1, mockClient);
}

TEST_F(TimedDoorTest, doorInitialization) {
    EXPECT_EQ(1, door.getTimeOut());
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, doorOpenedForTooLongException) {
    door.unlock();
    EXPECT_THROW({
        std::this_thread::sleep_for(std::chrono::seconds(2));
        DoorTimerAdapter adapter(door);
        adapter.Timeout();
    }, std::runtime_error);
}

TEST_F(TimedDoorTest, noTimeoutExceptionWhenClosed) {
    EXPECT_NO_THROW({
        DoorTimerAdapter adapter(door);
        adapter.Timeout();
    });
}

TEST_F(TimedDoorTest, doorStaysOpened) {
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}
