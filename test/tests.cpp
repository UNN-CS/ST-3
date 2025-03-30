// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

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

    void SetUp() override {
        door = new TimedDoor(5);
        timer = new Timer();
    }

    void TearDown() override {
        delete door;
        delete timer;
    }
};

TEST_F(TimedDoorTest, InitialStateIsClosed) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeout) {
    EXPECT_EQ(door->getTimeOut(), 5);
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
    door->unlock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, NoExceptionWhenClosed) {
    EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, ExceptionWhenOpen) {
    door->unlock();
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, AdapterTimeoutCalled) {
    MockTimerClient mockClient;
    TimedDoor testDoor(5);
    DoorTimerAdapter adapter(testDoor);

    EXPECT_CALL(mockClient, Timeout()).Times(1);
    timer->tregister(5, &mockClient);
}

TEST_F(TimedDoorTest, ConstructorSetsCorrectState) {
    TimedDoor newDoor(3);
    EXPECT_FALSE(newDoor.isDoorOpened());
    EXPECT_EQ(newDoor.getTimeOut(), 3);
}

TEST_F(TimedDoorTest, DoorInterfaceWorks) {
    MockDoor mockDoor;
    EXPECT_CALL(mockDoor, isDoorOpened()).WillOnce(::testing::Return(false));
    EXPECT_CALL(mockDoor, unlock()).Times(1);
    EXPECT_CALL(mockDoor, lock()).Times(1);

    mockDoor.unlock();
    mockDoor.lock();
    mockDoor.isDoorOpened();
}

TEST_F(TimedDoorTest, OperationSequence) {
    EXPECT_FALSE(door->isDoorOpened());
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
    EXPECT_NO_THROW(door->throwState());
}
