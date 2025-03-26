// Copyright 2025 Konkov Ivan

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"

using ::testing::_;
using ::testing::Throw;
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

class MockTimedDoor : public TimedDoor {
 public:
    MockTimedDoor(int timeout) : TimedDoor(timeout) {}
    MOCK_METHOD(bool, isDoorOpened, (), (override));
};

TEST(TimedDoorTest, DoorIsInitiallyClosed) {
    TimedDoor door(5);
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, UnlockOpensDoor) {
    TimedDoor door(5);
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

TEST(TimedDoorTest, LockClosesDoor) {
    TimedDoor door(5);
    door.unlock();
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, GetTimeoutReturnsCorrectValue) {
    TimedDoor door(10);
    EXPECT_EQ(door.getTimeOut(), 10);
}

TEST(TimedDoorTest, ThrowStateThrowsException) {
    TimedDoor door(5);
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, TimeoutOnOpenDoorThrows) {
    TimedDoor door(5);
    door.unlock();
    DoorTimerAdapter adapter(door);
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, TimeoutOnClosedDoorNoThrow) {
    TimedDoor door(5);
    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST(DoorTimerAdapterTest, AdapterCallsIsDoorOpened) {
    MockTimedDoor mockDoor(5);
    EXPECT_CALL(mockDoor, isDoorOpened()).WillOnce(Return(true));
    DoorTimerAdapter adapter(mockDoor);  // Теперь корректное приведение
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(TimerTest, TregisterCallsTimeout) {
    MockTimerClient mockClient;
    Timer timer;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    timer.tregister(0, &mockClient);
}

TEST(TimedDoorIntegrationTest, UnlockThenLockPreventsException) {
    TimedDoor door(5);
    door.unlock();
    door.lock();
    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
