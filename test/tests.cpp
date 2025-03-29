// Copyright 2025 PolinaSidorina

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <thread>
#include <chrono>
#include "TimedDoor.h"

using ::testing::Return;

#define TIME 2

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

TEST(DoorTest, test_1) {
    TimedDoor door(TIME);
    door.unlock();
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(DoorTest, test_2) {
    TimedDoor door(TIME);
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

TEST(DoorTest, test_3) {
    TimedDoor door(TIME);
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(DoorTest, test_4) {
    TimedDoor door(TIME);
    EXPECT_FALSE(door.isDoorOpened());
    EXPECT_NO_THROW(door.throwState());
}

TEST(DoorTest, test_5) {
    TimedDoor door(1);
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(TIME));
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST(DoorTest, test_6) {
    TimedDoor door(1);
    DoorTimerAdapter adapter(door);
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(TIME));
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(DoorTest, test_7) {
    TimedDoor door(1);
    door.unlock();
    door.lock();
    std::this_thread::sleep_for(std::chrono::seconds(TIME));
    EXPECT_NO_THROW(door.throwState());
}

TEST(DoorTest, test_8) {
    MockDoor mockDoor;
    DoorTimerAdapter adapter((TimedDoor&)mockDoor);
    EXPECT_CALL(mockDoor, isDoorOpened()).WillOnce(Return(false));
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST(DoorTest, test_9) {
    TimedDoor door(TIME);
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(TIME / 2));
    EXPECT_TRUE(door.isDoorOpened());
}

TEST(DoorTest, test_10) {
    MockDoor mockDoor;
    EXPECT_CALL(mockDoor, isDoorOpened()).WillOnce(Return(true));
    DoorTimerAdapter adapter((TimedDoor&)mockDoor);
    adapter.Timeout();
}
