// Copyright 2025 shvedovav

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
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

TEST_F(TimedDoorTest, doorStartsClosed) {
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, unlockOpensDoor) {
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, lockClosesDoor) {
    door.unlock();
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, cannotLockAlreadyClosedDoor) {
    EXPECT_THROW(door.lock(), std::logic_error);
}

TEST_F(TimedDoorTest, cannotUnlockAlreadyOpenedDoor) {
    door.unlock();
    EXPECT_THROW(door.unlock(), std::logic_error);
}

TEST_F(TimedDoorTest, stateThrowsCorrectly) {
    door.unlock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
    door.lock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, timerRejectsNullClient) {
    EXPECT_THROW(timer.tregister(5, nullptr), std::invalid_argument);
}

TEST_F(TimedDoorTest, timerRejectsInvalidTimeout) {
    EXPECT_THROW(timer.tregister(0, timerClient.get()), std::invalid_argument);
    EXPECT_THROW(timer.tregister(-1, timerClient.get()), std::invalid_argument);
}

TEST_F(TimedDoorTest, timerTriggersTimeoutCorrectly) {
    EXPECT_CALL(*timerClient, Timeout()).Times(1);
    timer.tregister(1, timerClient.get());
}

TEST_F(TimedDoorTest, doorRemainsClosedIfLockedBeforeTimeout) {
    door.unlock();
    timer.tregister(5, timerClient.get());
    std::this_thread::sleep_for(std::chrono::seconds(2));
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, doorThrowsAfterTimeoutIfNotLocked) {
    door.unlock();
    timer.tregister(3, timerClient.get());
    std::this_thread::sleep_for(std::chrono::seconds(4));
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, multipleLocksAndUnlocksWorkCorrectly) {
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, unlockingAndLockingMultipleTimesWorksCorrectly) {
    for (int i = 0; i < 3; ++i) {
        door.unlock();
        EXPECT_TRUE(door.isDoorOpened());
        door.lock();
        EXPECT_FALSE(door.isDoorOpened());
    }
}

TEST_F(TimedDoorTest, timerTriggersAfterExactTimeout) {
    EXPECT_CALL(*timerClient, Timeout()).Times(1);
    timer.tregister(2, timerClient.get());
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

TEST_F(TimedDoorTest, unlockingAnAlreadyUnlockedDoorFails) {
    door.unlock();
    EXPECT_THROW(door.unlock(), std::logic_error);
}

TEST_F(TimedDoorTest, lockingAnAlreadyLockedDoorFails) {
    EXPECT_THROW(door.lock(), std::logic_error);
}
