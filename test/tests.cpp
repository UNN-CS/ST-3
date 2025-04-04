// Copyright 2025 Salaev Vladislav

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"

using ::testing::AtLeast;

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
    Timer* timer;
    MockTimerClient* mockClient;

    void SetUp() override {
        door = new TimedDoor(2);
        timer = new Timer();
        mockClient = new MockTimerClient();
    }

    void TearDown() override {
        delete door;
        delete timer;
        delete mockClient;
    }
};

TEST_F(TimedDoorTest, DoorIsLockedInitially) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, CanUnlockDoor) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, CanLockDoor) {
    door->unlock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TimerTriggersTimeout) {
    EXPECT_CALL(*mockClient, Timeout()).Times(1);
    timer->tregister(1, mockClient);
}

TEST_F(TimedDoorTest, TimeoutThrowsWhenDoorOpen) {
    door->unlock();
    DoorTimerAdapter adapter(*door);
    EXPECT_THROW({
        timer->tregister(door->getTimeOut(), &adapter);
        }, std::runtime_error);
}

TEST_F(TimedDoorTest, TimeoutDoesNotThrowWhenDoorClosed) {
    door->lock();
    EXPECT_NO_THROW({
        timer->tregister(door->getTimeOut(), mockClient);
        });
}

TEST_F(TimedDoorTest, TimeoutCallsThrowState) {
    EXPECT_CALL(*mockClient, Timeout()).Times(1);
    timer->tregister(1, mockClient);
}

TEST_F(TimedDoorTest, AdapterTimeoutCallsThrowState) {
    DoorTimerAdapter adapter(*door);
    EXPECT_NO_THROW({
        adapter.Timeout();
        });
}

TEST_F(TimedDoorTest, DoorTimeoutIntegration) {
    door->unlock();
    DoorTimerAdapter adapter(*door);
    EXPECT_THROW({
        timer->tregister(door->getTimeOut(), &adapter);
        }, std::runtime_error);
    door->lock();
    EXPECT_NO_THROW({
        timer->tregister(door->getTimeOut(), &adapter);
        });
}

TEST_F(TimedDoorTest, MultipleTimeouts) {
    DoorTimerAdapter adapter(*door);
    door->unlock();
    EXPECT_THROW({
        timer->tregister(door->getTimeOut(), &adapter);
        }, std::runtime_error);
    door->lock();
    EXPECT_NO_THROW({
        timer->tregister(door->getTimeOut(), &adapter);
        });
    door->unlock();
    EXPECT_THROW({
        timer->tregister(door->getTimeOut(), &adapter);
        }, std::runtime_error);
}
