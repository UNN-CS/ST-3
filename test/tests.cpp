// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"

struct TestFuncDoor: public testing::Test {
    TimedDoor *door;
    void SetUp() {
        door = new TimedDoor(5);
    }
    void TearDown() {
        delete door;
    }
};

TEST_F(TestFuncDoor, StartStatus) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TestFuncDoor, StartOpen) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TestFuncDoor, StartLock) {
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TestFuncDoor, StartUnlockLock) {
    door->unlock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TestFuncDoor, getTimeOut) {
    EXPECT_EQ(door->getTimeOut(), 5);
}

using ::testing::AtLeast;
using ::testing::_;
using ::testing::Return;
using ::testing::AtLeast;
using ::testing::NiceMock;

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
        mockClient = new NiceMock<MockTimerClient>();
        mockDoor = new NiceMock<MockDoor>();
    }

    void TearDown() override {
        delete mockClient;
        delete mockDoor;
    }

    MockTimerClient* mockClient;
    MockDoor* mockDoor;
};

TEST_F(TimedDoorTest, LockShouldCloseDoor) {
    EXPECT_CALL(*mockDoor, lock()).Times(1);
    EXPECT_CALL(*mockDoor, isDoorOpened()).WillOnce(Return(false));

    mockDoor->lock();
    ASSERT_FALSE(mockDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockShouldOpenDoor) {
    EXPECT_CALL(*mockDoor, unlock()).Times(1);
    EXPECT_CALL(*mockDoor, isDoorOpened()).WillOnce(Return(true));

    mockDoor->unlock();
    ASSERT_TRUE(mockDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, ShouldCallTimeoutWhenDoorLeftOpen) {
    ON_CALL(*mockDoor, isDoorOpened()).WillByDefault(Return(true));

    EXPECT_CALL(*mockClient, Timeout()).Times(1);

    mockClient->Timeout();
}

TEST_F(TimedDoorTest, MultipleLockUnlockOperations) {
    EXPECT_CALL(*mockDoor, lock()).Times(3);
    EXPECT_CALL(*mockDoor, unlock()).Times(2);

    mockDoor->unlock();
    mockDoor->lock();
    mockDoor->unlock();
    mockDoor->lock();
    mockDoor->lock();
}

TEST_F(TimedDoorTest, ShouldAllowReopenAfterTimeout) {
    {
        EXPECT_CALL(*mockDoor, unlock()).Times(1);
        EXPECT_CALL(*mockClient, Timeout()).Times(1);

        mockDoor->unlock();
        mockClient->Timeout();
    }

    {
        EXPECT_CALL(*mockDoor, unlock()).Times(1);
        mockDoor->unlock();
    }
}
