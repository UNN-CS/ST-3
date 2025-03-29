// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <thread>
#include <chrono>
#include <memory>
#include "TimedDoor.h"

class MockThis : public TimerClient {
public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
protected:
    TimedDoor door{20};
    Timer timer{};
    std::unique_ptr<MockThis> mockClient;

    void SetUp() override {
        mockClient = std::make_unique<MockThis>();
    }
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

TEST_F(TimedDoorTest, LockAlreadyLockedThrows) {
    EXPECT_THROW(door.lock(), std::logic_error);
}

TEST_F(TimedDoorTest, UnlockAlreadyUnlockedThrows) {
    door.unlock();
    EXPECT_THROW(door.unlock(), std::logic_error);
}

TEST_F(TimedDoorTest, ThrowStateAlwaysThrows) {
    door.unlock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
    door.lock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, TimerNullClientThrows) {
    EXPECT_THROW(timer.tregister(5, nullptr), std::invalid_argument);
}

TEST_F(TimedDoorTest, TimerInvalidTimeoutThrows) {
    EXPECT_THROW(timer.tregister(0, mockClient.get()), std::invalid_argument);
    EXPECT_THROW(timer.tregister(-1, mockClient.get()), std::invalid_argument);
}

TEST_F(TimedDoorTest, TimerTriggersTimeout) {
    EXPECT_CALL(*mockClient, Timeout()).Times(1);
    timer.tregister(1, mockClient.get());
}

TEST_F(TimedDoorTest, DoorRemainsClosedIfLockedBeforeTimeout) {
    door.unlock();
    timer.tregister(3, mockClient.get());
    std::this_thread::sleep_for(std::chrono::seconds(1));
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, DoorThrowsAfterTimeoutIfNotLocked) {
    door.unlock();
    timer.tregister(2, mockClient.get());
    std::this_thread::sleep_for(std::chrono::seconds(3));
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, MultipleLocksAndUnlocks) {
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, MultipleUnlockLockSequence) {
    for (int i = 0; i < 3; i++) {
        door.unlock();
        EXPECT_TRUE(door.isDoorOpened());
        door.lock();
        EXPECT_FALSE(door.isDoorOpened());
    }
}

TEST_F(TimedDoorTest, TimerTriggersAfterExactTimeout) {
    EXPECT_CALL(*mockClient, Timeout()).Times(1);
    timer.tregister(2, mockClient.get());
    std::this_thread::sleep_for(std::chrono::seconds(2));
}
