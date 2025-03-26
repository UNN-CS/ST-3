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
    explicit MockTimedDoor(int timeout) : TimedDoor(timeout) {}
    MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class MockTimer : public Timer {
 public:
  MOCK_METHOD(void, tregister, (int, TimerClient*), (override));
  MOCK_METHOD(void, sleep, (int), (override));
};

TEST(TimedDoorTest, DoorIsInitiallyClosed) {
    TimedDoor door(5);
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, UnlockOpensDoor) {
    MockTimer mockTimer;
    TimedDoor door(5, mockTimer);
    
    EXPECT_CALL(mockTimer, tregister(5, _));
    door.unlock();
    
    EXPECT_TRUE(door.isDoorOpened());
}

TEST(TimedDoorTest, LockClosesDoor) {
    MockTimer mockTimer;
    TimedDoor door(5);
    
    EXPECT_CALL(mockTimer, tregister(5, _));
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
    MockTimer mockTimer;
    TimedDoor door(5);
    
    EXPECT_CALL(mockTimer, tregister(5, _));
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
    DoorTimerAdapter adapter(mockDoor);
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(TimerTest, TregisterCallsTimeout) {
    MockTimerClient mockClient;
    Timer timer;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    timer.tregister(0, &mockClient);
}

TEST(TimedDoorIntegrationTest, UnlockThenLockPreventsException) {
    MockTimer mockTimer;
    TimedDoor door(5);
    
    EXPECT_CALL(mockTimer, tregister(5, _));
    door.unlock();
    
    door.lock();
    
    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
}
