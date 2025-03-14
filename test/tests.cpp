// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"

class MockTimer : public Timer {
public:
    MOCK_METHOD(void, tregister, (int, TimerClient*), (override));
    void sleep(int) override {} // Пустая реализация для тестов
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
    MockTimer* mockTimer;
    const int timeout = 5;

    void SetUp() override {
        mockTimer = new MockTimer();
        door = new TimedDoor(timeout, mockTimer);
    }

    void TearDown() override {
        delete door; // mockTimer будет удален в деструкторе TimedDoor
    }
};

TEST_F(TimedDoorTest, InitialStateTest) {
    EXPECT_FALSE(door->isDoorOpened());
    EXPECT_EQ(door->getTimeOut(), timeout);
}

TEST_F(TimedDoorTest, LockTest) {
    EXPECT_CALL(*mockTimer, tregister(timeout, testing::_)).Times(1);
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockTest) {
    EXPECT_CALL(*mockTimer, tregister(timeout, testing::_)).Times(1);
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
    MockTimer* mockTimer = new MockTimer();
    TimedDoor door(5, mockTimer);
    DoorTimerAdapter adapter(door);
    door.unlock();
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, NoTimeoutWhenLockedTest) {
    MockTimer* mockTimer = new MockTimer();
    TimedDoor door(5, mockTimer);
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
