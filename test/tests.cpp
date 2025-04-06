// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"
#include <stdexcept>
#include <thread>

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

class DoorAndAdapterTestF : public testing::Test {
public:
    TimedDoor* door;
    int standard_timeout = 100;
    DoorTimerAdapter *adapter;

    void SetUp() {
        door = new TimedDoor(standard_timeout);
        adapter = new DoorTimerAdapter(*door);
    }
    void TearDown() {
        delete door;
        delete adapter;
    }
}

TEST_F(DoorAndAdapterTestF, isDoorOpenedWorksCorrect) {
    ASSERT_FALSE(door->isDoorOpened());
}

TEST_F(DoorAndAdapterTestF, unlockWorksCorrect) {
    door->unlock();
    ASSERT_TRUE(door->isDoorOpened());
}

TEST_F(DoorAndAdapterTestF, unlockCallsTimeout) {
    MockTimerClient mock_timer_client;
    EXPECT_CALL(mock_timer_client, Timeout()).Times(1);
    door->unlock();
}

TEST_F(DoorAndAdapterTestF, lockWorksCorrect) {
    door->unlock();
    door->lock();
    ASSERT_FALSE(door->isDoorOpened());
}

TEST_F(DoorAndAdapterTestF, getTimeOutWorksCorrect) {
    ASSERT_EQ(door->getTimeOut(), standard_timeout);
}

TEST_F(DoorAndAdapterTestF, throwStateThrowsException) {
    ASSERT_THROW(door->throwState(), std::exception);
}

TEST_F(DoorAndAdapterTestF, unlockThrowsException) {
    ASSERT_THROW(door->unlock(), std::exception);
}

TEST_F(DoorAndAdapterTestF, notThrowsWhenClosedInsideTimeInterval) {
    std::exception_ptr eptr = nullptr;

    std::thread unlock_thread([&]() {
        try {
            door->unlock();
        } catch (...) {
            eptr = std::current_exception();
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(standard_timeout / 2));
    door->lock();
    unlock_thread.join();

    ASSERT_EQ(eptr, nullptr);
}

TEST_F(DoorAndAdapterTestF, throwsWhenClosedOutsideTimeInterval) {
    std::exception_ptr eptr = nullptr;

    std::thread unlock_thread([&]() {
        try {
            door->unlock();
        } catch (...) {
            eptr = std::current_exception();
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(standard_timeout * 2));
    door->lock();
    unlock_thread.join();

    ASSERT_NE(eptr, nullptr);
}

TEST_F(DoorAndAdapterTestF, timeoutCallsIsDoorOpened) {
    MockDoor mock_door;
    EXPECT_CALL(mock_door, isDoorOpened()).Times(1);
    adapter->Timeout();
}
