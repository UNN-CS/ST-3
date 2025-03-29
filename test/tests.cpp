// Copyright 2025 Alexey Gromov

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"

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
    void SetUp() override {
        mockTimerClient = new MockTimerClient();
        mockDoor = new MockDoor();
        timedDoor = new TimedDoor(1);
    }

    void TearDown() override {
        delete mockTimerClient;
        delete mockDoor;
        delete timedDoor;
    }

    MockTimerClient* mockTimerClient;
    MockDoor* mockDoor;
    TimedDoor* timedDoor;
};

TEST_F(TimedDoorTest, DoorInitiallyClosed) {
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockSetsDoorOpen) {
    timedDoor->unlock();
    EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, LockSetsDoorClosed) {
    timedDoor->unlock();
    timedDoor->lock();
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeOutReturnsCorrectValue) {
    EXPECT_EQ(timedDoor->getTimeOut(), 1);
}

TEST_F(TimedDoorTest, ThrowsExceptionIfDoorOpenAfterTimeout) {
    timedDoor->unlock();
    EXPECT_THROW(timedDoor->getTimeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, NoExceptionIfDoorClosedAfterTimeout) {
    timedDoor->unlock();
    timedDoor->lock();
    EXPECT_NO_THROW(timedDoor->getTimeout());
}

TEST_F(TimedDoorTest, MockDoorLockCalled) {
    EXPECT_CALL(*mockDoor, lock()).Times(1);
    mockDoor->lock();
}

TEST_F(TimedDoorTest, MockDoorUnlockCalled) {
    EXPECT_CALL(*mockDoor, unlock()).Times(1);
    mockDoor->unlock();
}

TEST_F(TimedDoorTest, MockDoorIsDoorOpenedCalled) {
    EXPECT_CALL(*mockDoor, isDoorOpened()).Times(1);
    mockDoor->isDoorOpened();
}

TEST_F(TimedDoorTest, TimerRegistersClient) {
    Timer timer;
    EXPECT_CALL(*mockTimerClient, Timeout()).Times(1);
    timer.registerTimer(1, mockTimerClient);
}