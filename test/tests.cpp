// Copyright 2025 Konkov Ivan

#include <thread>
#include <chrono>
#include "TimedDoor.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Throw;

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
    void SetUp() override {
        door = new TimedDoor(5);
        timer = new Timer();
    }

    void TearDown() override {
        delete door;
        delete timer;
    }

    TimedDoor* door;
    Timer* timer;
};

TEST_F(TimedDoorTest, InitialStateIsLocked) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
    door->unlock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutThrowsWhenOpen) {
    door->unlock();
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, NoExceptionWhenClosed) {
    door->lock();
    EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, AdapterCallsTimeout) {
    MockTimerClient client;
    EXPECT_CALL(client, Timeout()).Times(1);
    
    Timer timer;
    timer.tregister(1, &client);
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

TEST_F(TimedDoorTest, TimerTriggersClientTimeout) {
    testing::NiceMock<MockTimerClient> client;
    EXPECT_CALL(client, Timeout()).Times(AtLeast(1));
    
    timer->tregister(1, &client);
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

TEST_F(TimedDoorTest, UnlockTriggersAdapter) {
    MockTimerClient client;
    EXPECT_CALL(client, Timeout()).Times(1);
    
    TimedDoor door(5);
    door.unlock();
}

TEST_F(TimedDoorTest, MultipleUnlocks) {
    door->unlock();
    door->lock();
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, ExceptionAfterTimeout) {
    door->unlock();
    timer->tregister(5, door->getAdapter());
    
    EXPECT_THROW({
        std::this_thread::sleep_for(std::chrono::seconds(6));
    }, std::runtime_error);
}

DoorTimerAdapter* TimedDoor::getAdapter() { 
    return adapter; 
}
