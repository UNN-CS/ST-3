// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"

using ::testing::Exactly;

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

    void SetUp() override {
        door = new TimedDoor(1);
    }

    void TearDown() override {
        delete door;
    }
};

//1
TEST_F(TimedDoorTest, InitiallyClosed) {
    EXPECT_FALSE(door->isDoorOpened());
}

//2
TEST_F(TimedDoorTest, UnlockOpensDoor) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

//3
TEST_F(TimedDoorTest, LockClosesDoor) {
    door->unlock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

//4
TEST_F(TimedDoorTest, GetTimeoutWorks) {
    EXPECT_EQ(door->getTimeOut(), 1);
}

//5
TEST_F(TimedDoorTest, ThrowStateThrows) {
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

//6
TEST(AdapterTest, ThrowsIfDoorOpen) {
    TimedDoor door(1);
    DoorTimerAdapter adapter(door);

    door.unlock();

    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

//7
TEST(AdapterTest, NoThrowIfClosed) {
    TimedDoor door(1);
    DoorTimerAdapter adapter(door);

    door.lock();

    EXPECT_NO_THROW(adapter.Timeout());
}

//8 (MOCK)
TEST(MockTest, TimerCallsTimeout) {
    MockTimerClient mock;

    EXPECT_CALL(mock, Timeout()).Times(Exactly(1));

    Timer timer;
    timer.tregister(0, &mock);
}

//9 (MOCK)
TEST(MockTest, DoorMethodsCalled) {
    MockDoor mockDoor;

    EXPECT_CALL(mockDoor, unlock()).Times(1);
    EXPECT_CALL(mockDoor, lock()).Times(1);

    mockDoor.unlock();
    mockDoor.lock();
}

//10
TEST(TimerTest, NoCrashWithNullClient) {
    Timer timer;
    EXPECT_NO_THROW(timer.tregister(0, nullptr));
}