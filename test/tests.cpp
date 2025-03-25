// test/tests.cpp
// Copyright 2021 GHA Test Team
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <thread>

#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
    void SetUp() override {
        timerClient = std::make_unique<MockTimerClient>();
    }

    TimedDoor door{5};
    Timer timer;
    std::unique_ptr<MockTimerClient> timerClient;
};

TEST_F(TimedDoorTest, DoorStartsClosed) {
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
    door.unlock();
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, CannotLockAlreadyClosedDoor) {
    EXPECT_THROW(door.lock(), std::logic_error);
}

TEST_F(TimedDoorTest, CannotUnlockAlreadyOpenedDoor) {
    door.unlock();
    EXPECT_THROW(door.unlock(), std::logic_error);
}

TEST_F(TimedDoorTest, ThrowsErrorWhenDoorRemainsOpenTooLong) {
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(6));
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, TimerRejectsNullClient) {
    EXPECT_THROW(timer.tregister(5, nullptr), std::invalid_argument);
}

TEST_F(TimedDoorTest, TimerRejectsInvalidTimeout) {
    EXPECT_THROW(timer.tregister(0, timerClient.get()), std::invalid_argument);
    EXPECT_THROW(timer.tregister(-1, timerClient.get()), std::invalid_argument);
}

TEST_F(TimedDoorTest, TimerTriggersTimeoutCorrectly) {
    EXPECT_CALL(*timerClient, Timeout()).Times(1);
    timer.tregister(1, timerClient.get());
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

TEST_F(TimedDoorTest, DoorRemainsClosedIfLockedBeforeTimeout) {
    door.unlock();
    timer.tregister(5, timerClient.get());
    std::this_thread::sleep_for(std::chrono::seconds(2));
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, DoorThrowsAfterTimeoutIfNotLocked) {
    door.unlock();
    timer.tregister(3, timerClient.get());
    std::this_thread::sleep_for(std::chrono::seconds(4));
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, MultipleLocksAndUnlocksWorkCorrectly) {
    for (int i = 0; i < 3; ++i) {
        door.unlock();
        EXPECT_TRUE(door.isDoorOpened());
        door.lock();
        EXPECT_FALSE(door.isDoorOpened());
    }
}

TEST_F(TimedDoorTest, TimerTriggersAfterExactTimeout) {
    EXPECT_CALL(*timerClient, Timeout()).Times(1);
    timer.tregister(2, timerClient.get());
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

TEST_F(TimedDoorTest, UnlockingAnAlreadyUnlockedDoorFails) {
    door.unlock();
    EXPECT_THROW(door.unlock(), std::logic_error);
}

TEST_F(TimedDoorTest, LockingAnAlreadyLockedDoorFails) {
    EXPECT_THROW(door.lock(), std::logic_error);
}
