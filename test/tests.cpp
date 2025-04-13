// Copyright 2025 Vasilev Sergey

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"
#include <stdexcept>
#include <chrono>
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

class MockTimer : public Timer {
public:
    void sleep(int seconds) override {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    MOCK_METHOD(void, tregister, (int, TimerClient*), (override));
};

class TimedDoorTest : public ::testing::Test {
protected:
    TimedDoor* door;
    
    void SetUp() override {
        door = new TimedDoor(1);
    }
    
    void TearDown() override {
        delete door;
    }
};

TEST_F(TimedDoorTest, InitialStateTest) {
    EXPECT_FALSE(door->isDoorOpened());
    EXPECT_EQ(door->getTimeOut(), 1);
}

TEST_F(TimedDoorTest, UnlockTest) {
    EXPECT_FALSE(door->isDoorOpened());
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockTest) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockExceptionTest) {
    door->unlock();
    EXPECT_THROW(door->unlock(), std::logic_error);
}

TEST_F(TimedDoorTest, LockExceptionTest) {
    EXPECT_THROW(door->lock(), std::logic_error);
}

TEST_F(TimedDoorTest, ThrowStateTest) {
    EXPECT_THROW(door->throwState(), std::runtime_error);
    door->unlock();
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST(AdapterTest, TimeoutWhenDoorOpen) {
    TimedDoor door(1);
    door.unlock();
    DoorTimerAdapter adapter(door);
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(AdapterTest, TimeoutWhenDoorClosed) {
    TimedDoor door(1);
    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST(TimerTest, RegisterClient) {
    MockTimerClient client;
    Timer timer;
    
    EXPECT_CALL(client, Timeout())
        .Times(1);
    
    timer.sleep = [](int) {};
    
    timer.tregister(1, &client);
}

TEST(TimerTest, InvalidParameters) {
    Timer timer;
    EXPECT_THROW(timer.tregister(-1, nullptr), std::invalid_argument);
}

class TimerForTest : public Timer {
public:
    void sleep(int) override {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
};

TEST(IntegrationTest, DoorTimerIntegration) {
    TimedDoor door(2);
    DoorTimerAdapter adapter(door);
    TimerForTest timer;
    
    EXPECT_FALSE(door.isDoorOpened());
    
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
    
    EXPECT_THROW(timer.tregister(2, &adapter), std::runtime_error);
    
    door.lock();
    
    EXPECT_NO_THROW(timer.tregister(2, &adapter));
}
