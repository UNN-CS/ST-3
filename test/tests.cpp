// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"

class MockTimer : public Timer {
public:
    MOCK_METHOD(void, tregister, (int, TimerClient*), (override));
    // Переопределяем sleep, чтобы он ничего не делал в тестах
    void sleep(int) override {}
};

class MockDoor : public Door {
public:
    MOCK_METHOD(void, lock, (), (override));
    MOCK_METHOD(void, unlock, (), (override));
    MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class TimedDoorTest : public ::testing::Test {
protected:
    TimedDoor* door;
    const int timeout = 5;

    void SetUp() override {
        door = new TimedDoor(timeout);
    }

    void TearDown() override {
        delete door;
    }
};

TEST_F(TimedDoorTest, InitialStateTest) {
    EXPECT_FALSE(door->isDoorOpened());
    EXPECT_EQ(door->getTimeOut(), timeout);
}

TEST_F(TimedDoorTest, LockTest) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockTest) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutValueTest) {
    EXPECT_EQ(door->getTimeOut(), 5);
}

TEST_F(TimedDoorTest, ExceptionOnTimeoutTest) {
    door->unlock();
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, NoExceptionWhenLockedTest) {
    door->lock();
    EXPECT_NO_THROW(door->throwState());
}

TEST(DoorTimerAdapterTest, TimeoutCallTest) {
    TimedDoor door(5);
    DoorTimerAdapter adapter(door);
    door.unlock();
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, NoTimeoutWhenLockedTest) {
    TimedDoor door(5);
    DoorTimerAdapter adapter(door);
    door.lock();
    EXPECT_NO_THROW(adapter.Timeout());
}

class MockTimerClient : public TimerClient {
public:
    MOCK_METHOD(void, Timeout, (), (override));
};

TEST(TimerTest, RegisterCallbackTest) {
    MockTimer timer;
    MockTimerClient client;
    EXPECT_CALL(timer, tregister(1, &client)).Times(1);
    timer.tregister(1, &client);
}

TEST(TimerTest, TimeoutCallbackTest) {
    MockTimer timer;
    MockTimerClient client;
    EXPECT_CALL(timer, tregister(5, &client)).Times(1);
    timer.tregister(5, &client);
}
