// Copyright 2025 Zinoviev Alexander

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"

using ::testing::_;
using ::testing::Return;

class MockTimedDoor : public TimedDoor {
 public:
  explicit MockTimedDoor(int timeout) : TimedDoor(timeout) {}
  MOCK_METHOD(bool, isDoorOpened, (), (override));
  MOCK_METHOD(void, throwState, (), ());
};

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

TEST(DoorTimerAdapterTest, TimeoutCallsThrowStateWhenDoorIsOpen) {
    MockTimedDoor door(1000);
    EXPECT_CALL(door, isDoorOpened()).WillOnce(Return(true));
    EXPECT_CALL(door, throwState()).Times(1);
    DoorTimerAdapter adapter(door);
    adapter.Timeout();
}

TEST(DoorTimerAdapterTest, TimeoutDoesNotCallThrowStateWhenDoorIsClosed) {
    MockTimedDoor door(1000);
    EXPECT_CALL(door, isDoorOpened()).WillOnce(Return(false));
    EXPECT_CALL(door, throwState()).Times(0);
    DoorTimerAdapter adapter(door);
    adapter.Timeout();
}

TEST(TimedDoorTest, IsInitiallyClosed) {
    TimedDoor door(1000);
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, UnlockOpensDoor) {
    TimedDoor door(1000);
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

TEST(TimedDoorTest, LockClosesDoor) {
    TimedDoor door(1000);
    door.unlock();
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, ThrowAfterTimeoutIfOpen) {
    TimedDoor door(0);
    EXPECT_THROW(door.unlock(), std::runtime_error);
}

TEST(TimedDoorTest, GetTimeoutReturnsCorrectValue) {
    TimedDoor door(500);
    EXPECT_EQ(door.getTimeOut(), 500);
}

TEST(TimedDoorTest, ThrowStateThrowsException) {
    TimedDoor door(500);
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST(TimerTest, CallsTimeoutAfterTime) {
    MockTimerClient client;
    EXPECT_CALL(client, Timeout()).Times(1);
    Timer timer;
    timer.tregister(0, &client);
}

TEST(TimedDoorIntegrationTest, UnlockRegistersTimer) {
    TimedDoor door(0);
    EXPECT_THROW(door.unlock(), std::runtime_error);
}
