#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <thread>
#include "TimedDoor.h"


class MockTimerClient : public TimerClient {
public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
protected:
    const int testTimeout = 1; // 1 секунда (как в вашей реализации)
    TimedDoor* door;
    MockTimerClient* mockClient;

    void SetUp() override {
        door = new TimedDoor(testTimeout);
        mockClient = new MockTimerClient();
    }

    void TearDown() override {
        delete door;
        delete mockClient;
    }
};

TEST_F(TimedDoorTest, InitialStateIsClosed) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensTheDoor) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesTheDoor) {
    door->unlock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, ThrowStateThrowsWhenDoorIsOpen) {
    door->unlock();
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, NoExceptionWhenDoorIsClosed) {
    door->lock();
    EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, AdapterCallsThrowStateOnTimeout) {
    DoorTimerAdapter adapter(*door);
    door->unlock();
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, GetTimeoutReturnsCorrectValue) {
    EXPECT_EQ(door->getTimeOut(), testTimeout);
}

TEST_F(TimedDoorTest, CanReopenAfterClosing) {
    door->unlock();
    door->lock();
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, CanCloseMultipleTimes) {
    door->lock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TimerResetWhenLockedBeforeTimeout) {
    door->unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    door->lock();
    EXPECT_NO_THROW(door->throwState());
}