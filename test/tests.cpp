// Copyright 2025 Korneeva Ekaterina

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <stdexcept>
#include <thread>
#include <chrono>
#include "TimedDoor.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Return;

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
        door = new TimedDoor(100);
    }

    void TearDown() override {
        delete door;
    }

    TimedDoor* door;
};

TEST_F(TimedDoorTest, InitialStateIsLocked) {
    EXPECT_FALSE(door->isDoorOpened());
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

TEST_F(TimedDoorTest, TimeoutThrowsWhenDoorOpened) {
    door->unlock();
    DoorTimerAdapter adapter(*door);
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, TimeoutDoesNotThrowWhenDoorClosed) {
    door->lock();
    DoorTimerAdapter adapter(*door);
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST_F(TimedDoorTest, TimerRegistrationWithMock) {
    MockTimerClient mockClient;
    Timer timer;

    EXPECT_CALL(mockClient, Timeout()).Times(1);
    timer.tregister(50, &mockClient);
    std::this_thread::sleep_for(std::chrono::milliseconds(70));
}

TEST_F(TimedDoorTest, InvalidTimerParametersThrow) {
    Timer timer;
    MockTimerClient mockClient;

    EXPECT_THROW(timer.tregister(-1, &mockClient), std::invalid_argument);
    EXPECT_THROW(timer.tregister(100, nullptr), std::invalid_argument);
}

TEST_F(TimedDoorTest, ThrowStateThrowsCorrectException) {
    EXPECT_THROW(door->throwState(), std::runtime_error);
    try {
        door->throwState();
    }
    catch (const std::runtime_error& e) {
        EXPECT_STREQ(e.what(), "Door opened too long");
    }
}

TEST_F(TimedDoorTest, GetTimeoutReturnsCorrectValue) {
    EXPECT_EQ(door->getTimeOut(), 100);
}

TEST_F(TimedDoorTest, RealTimerWithShortTimeoutThrows) {
    TimedDoor fastDoor(10);
    fastDoor.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
    DoorTimerAdapter adapter(fastDoor);
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, CloseBeforeTimeoutPreventsThrow) {
    TimedDoor door(100);
    door.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    door.lock();
    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST_F(TimedDoorTest, DoubleUnlockDoesNotThrow) {
    door->unlock();
    EXPECT_NO_THROW(door->unlock());
}

TEST_F(TimedDoorTest, DoorStaysOpenedBeforeTimeout) {
    door->unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_TRUE(door->isDoorOpened());
}
