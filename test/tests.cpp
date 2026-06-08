// Copyright 2021 GHA Test Team
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdexcept>
#include "TimedDoor.h"

using ::testing::Exactly;
using ::testing::Return;

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door {
 public:
    MOCK_METHOD(void,  lock,         (), (override));
    MOCK_METHOD(void,  unlock,       (), (override));
    MOCK_METHOD(bool,  isDoorOpened, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
    TimedDoor* door;

    void SetUp() override {
        door = new TimedDoor(5);
    }

    void TearDown() override {
        delete door;
        door = nullptr;
    }
};

TEST_F(TimedDoorTest, DoorIsClosedAfterCreation) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockSetsDoorToOpen) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockSetsDoorToClosed) {
    door->unlock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutValueMatchesConstructorArg) {
    EXPECT_EQ(door->getTimeOut(), 5);
}

TEST_F(TimedDoorTest, ThrowStateAlwaysThrowsRuntimeError) {
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, TimeoutThrowsWhenDoorIsOpen) {
    TimedDoor td(3);
    DoorTimerAdapter adapter(td);
    td.unlock();
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, TimeoutNoThrowWhenDoorIsClosed) {
    TimedDoor td(3);
    DoorTimerAdapter adapter(td);
    td.lock();
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST(DoorTimerAdapterTest, TimeoutThrowsExactExceptionType) {
    TimedDoor td(1);
    DoorTimerAdapter adapter(td);
    td.unlock();
    try {
        adapter.Timeout();
        FAIL() << "Expected std::runtime_error";
    } catch (const std::runtime_error&) {
    } catch (...) {
        FAIL() << "Expected std::runtime_error, got something else";
    }
}

TEST(TimerMockTest, RegisterCallsTimeoutExactlyOnce) {
    MockTimerClient mockClient;
    EXPECT_CALL(mockClient, Timeout()).Times(Exactly(1));
    Timer timer;
    timer.tregister(0, &mockClient);
}

TEST(DoorMockTest, UnlockAndLockEachCalledOnce) {
    MockDoor mockDoor;
    EXPECT_CALL(mockDoor, unlock()).Times(Exactly(1));
    EXPECT_CALL(mockDoor, lock()).Times(Exactly(1));
    mockDoor.unlock();
    mockDoor.lock();
}

TEST(TimerTest, RegisterWithNullClientDoesNotCrash) {
    Timer timer;
    EXPECT_NO_THROW(timer.tregister(0, nullptr));
}

TEST(DoorMockTest, MockReturnsExpectedOpenState) {
    MockDoor mockDoor;
    EXPECT_CALL(mockDoor, isDoorOpened())
        .Times(Exactly(1))
        .WillOnce(Return(true));
    EXPECT_TRUE(mockDoor.isDoorOpened());
}
