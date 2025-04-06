// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"
#include <stdexcept>
#include <thread>

class DoorAndAdapterTestF : public testing::Test {
 public:
    TimedDoor* door;
    int standard_timeout = 100;

    void SetUp() {
        door = new TimedDoor(standard_timeout);
    }
    void TearDown() {
        delete door;
    }
};

TEST_F(DoorAndAdapterTestF, isDoorOpenedWorksCorrect) {
    ASSERT_FALSE(door->isDoorOpened());
}

TEST_F(DoorAndAdapterTestF, unlockWorksCorrect) {
    try {
        door->unlock();
    } catch (...) {}
    ASSERT_TRUE(door->isDoorOpened());
}

TEST_F(DoorAndAdapterTestF, lockWorksCorrect) {
    try {
        door->unlock();
    } catch (...) {}
    door->lock();
    ASSERT_FALSE(door->isDoorOpened());
}

TEST_F(DoorAndAdapterTestF, getTimeOutWorksCorrect) {
    ASSERT_EQ(door->getTimeOut(), standard_timeout);
}

TEST_F(DoorAndAdapterTestF, throwStateThrowsException) {
    ASSERT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(DoorAndAdapterTestF, unlockThrowsException) {
    ASSERT_THROW(door->unlock(), std::runtime_error);
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

    std::this_thread::sleep_for(
        std::chrono::milliseconds(standard_timeout / 2));
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

    std::this_thread::sleep_for(
        std::chrono::milliseconds(standard_timeout * 2));
    door->lock();
    unlock_thread.join();

    ASSERT_NE(eptr, nullptr);
}

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

TEST(MockTimerClientTest, tregisterCallsTimeout) {
    MockTimerClient mock_timer_client;
    EXPECT_CALL(mock_timer_client, Timeout()).Times(1);
    Timer timer;
    timer.tregister(100, &mock_timer_client);
}

class MockDoor : public Door {
 public:
    MOCK_METHOD(void, lock, (), (override));
    MOCK_METHOD(void, unlock, (), (override));
    MOCK_METHOD(bool, isDoorOpened, (), (override));
};

TEST(MockDoorTest, mockDoorCalls) {
    MockDoor mock_door;

    EXPECT_CALL(mock_door, lock()).Times(1);
    EXPECT_CALL(mock_door, unlock()).Times(1);
    EXPECT_CALL(mock_door, isDoorOpened())
        .Times(1)
        .WillOnce(testing::Return(true));

    mock_door.lock();
    mock_door.unlock();

    EXPECT_TRUE(mock_door.isDoorOpened());
}
