// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"

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
    inline static constexpr int tmo = 5;
    TimedDoor door{tmo};
    DoorTimerAdapter adapter{door};
    Timer timer;
};


TEST_F(TimedDoorTest, can_unlock) {
    door.unlock();
    ASSERT_TRUE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, can_lock) {
    door.unlock();
    ASSERT_TRUE(door.isDoorOpened());
    door.lock();
    ASSERT_FALSE(door.isDoorOpened());
}
TEST_F(TimedDoorTest, initially_closed) {
    ASSERT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, doesnt_throw_an_exception_when_door_is_closed) {
    door.lock();
    EXPECT_NO_THROW(adapter.Timeout());
}
TEST_F(TimedDoorTest, throws_an_exception_when_door_is_opened) {
    door.unlock();
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, timer_tregister_is_called) {
    MockTimerClient mockClient;
    Timer timer;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    timer.tregister(1, &mockClient);
}

TEST_F(TimedDoorTest, can_be_locked_after_timeout) {
    door.unlock();
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);

    door.lock();
    EXPECT_NO_THROW(adapter.Timeout());
}
TEST_F(TimedDoorTest, unlock_is_idempodent) {
    door.unlock();
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, lock_is_idempodent) {
    door.lock();
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, returns_correct_timeout) {
    ASSERT_EQ(door.getTimeOut(), tmo);
}
