// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"

using ::testing::Exactly;
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

    void SetUp() override {
        door = new TimedDoor(1);
    }

    void TearDown() override {
        delete door;
    }
};

TEST_F(TimedDoorTest, DoorInitiallyClosed) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
    try {
        door->unlock();
    } catch (...) {
    }

    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
    try {
        door->unlock();
    } catch (...) {
    }

    door->lock();

    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutValueStored) {
    TimedDoor d(50);
    EXPECT_EQ(d.getTimeOut(), 50);
}

TEST_F(TimedDoorTest, ThrowStateThrowsException) {
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST(TimerTest, TimerCallsTimeout) {
    Timer timer;
    MockTimerClient client;

    EXPECT_CALL(client, Timeout())
        .Times(1);

    timer.tregister(1, &client);
}

TEST(AdapterTest, AdapterThrowsWhenDoorOpened) {
    TimedDoor door(1000);

    door.lock();

    try {
        door.unlock();
    } catch (...) {
    }

    DoorTimerAdapter adapter(door);

    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(AdapterTest, AdapterDoesNotThrowWhenDoorClosed) {
    TimedDoor door(1000);
    door.lock();

    DoorTimerAdapter adapter(door);

    EXPECT_NO_THROW(adapter.Timeout());
}

TEST_F(TimedDoorTest, OpenedDoorThrowsAfterTimeout) {
    EXPECT_THROW(door->unlock(), std::runtime_error);
}

TEST(TimedDoorIntegration, ClosedDoorDoesNotThrow) {
    TimedDoor door(100);

    door.lock();

    DoorTimerAdapter adapter(door);

    EXPECT_NO_THROW(adapter.Timeout());
}

TEST(MockDoorTest, MethodsAreCalled) {
    MockDoor door;

    EXPECT_CALL(door, unlock()).Times(1);

    EXPECT_CALL(door, lock()).Times(1);

    door.unlock();
    door.lock();
}
