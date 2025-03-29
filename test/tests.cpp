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

TEST(DoorTest, test1) {
    TimedDoor door(TIME);
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(DoorTest, test2) {
    TimedDoor door(TIME);
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

TEST(DoorTest, test3) {
    TimedDoor door(TIME);
    door.unlock();
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(DoorTest, test4) {
    MockTimerClient mockClient;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    Timer timer;
    timer.tregister(TIME, &mockClient);
    std::this_thread::sleep_for(std::chrono::seconds(TIME));
}

TEST(DoorTest, test5) {
    TimedDoor door(1);
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(TIME));
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST(DoorTest, test6) {
    TimedDoor door(1);
    door.unlock();
    door.lock();
    std::this_thread::sleep_for(std::chrono::seconds(TIME));
    EXPECT_NO_THROW(door.throwState());
}

TEST(DoorTest, test7) {
    TimedDoor door(1);
    DoorTimerAdapter adapter(door);
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(TIME));
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(DoorTest, test8) {
    MockTimerClient mockClient;
    Timer timer;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    timer.tregister(1, &mockClient);
}

TEST(DoorTest, test9) {
    TimedDoor door(TIME);
    EXPECT_FALSE(door.isDoorOpened());
    EXPECT_NO_THROW(door.throwState());
}

TEST(DoorTest, test10) {
    MockDoor mockDoor;
    DoorTimerAdapter adapter((TimedDoor&)mockDoor);
    EXPECT_CALL(mockDoor, isDoorOpened()).WillOnce(Return(false));
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST(DoorTest, test11) {
    TimedDoor door(TIME);
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(TIME / 2));
    EXPECT_TRUE(door.isDoorOpened());
}

TEST(DoorTest, test12) {
    TimedDoor door(TIME);
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(TIME * 2));
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(DoorTest, test13) {
    MockDoor mockDoor;
    EXPECT_CALL(mockDoor, isDoorOpened()).WillOnce(Return(true));
    DoorTimerAdapter adapter((TimedDoor&)mockDoor);
    adapter.Timeout();
}

TEST(DoorTest, test14) {
    MockDoor mockDoor;
    EXPECT_CALL(mockDoor, isDoorOpened()).WillOnce(Return(false));
    DoorTimerAdapter adapter((TimedDoor&)mockDoor);
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(DoorTest, test15) {
    MockDoor mockDoor;
    DoorTimerAdapter adapter((TimedDoor&)mockDoor);
    adapter.Timeout();
    EXPECT_CALL(mockDoor, lock()).Times(1);
}
