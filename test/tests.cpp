// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"
#include <stdexcept>
#include <thread>

class MockDoor : public Door {
 public:
    MOCK_METHOD(void, lock, (), (override));
    MOCK_METHOD(void, unlock, (), (override));
    MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class TestTimer : public Timer {
 public:
    void tregister(int timeout, TimerClient *client) {
        client->Timeout();
    }
};

class TestTimedDoor : public TimedDoor {
 public:
    using TimedDoor::TimedDoor;

    MOCK_METHOD(void, throwState, (), ());
};

class DoorTest : public ::testing::Test {
 protected:
    TestTimedDoor *door;

    void SetUp() override {
        door = new TestTimedDoor(100);
    }

    void TearDown() override {
        delete door;
    }
};

TEST_F(DoorTest, LockDoor) {
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(DoorTest, GetTimeoutReturnsCorrectValue) {
    EXPECT_EQ(door->getTimeOut(), 100);
}

TEST_F(DoorTest, TimeoutNoThrowIfDoorClosed) {
    door->lock();
    EXPECT_NO_THROW({
        DoorTimerAdapter adapter(*door);
        adapter.Timeout();
    });
}

TEST(TimerTest, TimerCallsTimeout) {
    MockTimerClient client;
    Timer timer;
    EXPECT_CALL(client, Timeout()).Times(1);
    timer.tregister(10, &client);
}

TEST(AdapterTest, DoorOpenThrowsException) {
    TimedDoor door(100);
    EXPECT_THROW({
        door.unlock();
        std::this_thread::sleep_for
        (std::chrono::milliseconds(200)); }, std::runtime_error);
}

TEST(AdapterTest, AdapterDoesNotThrowWhenClosed) {
    TimedDoor door(100);
    door.lock();
    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST_F(DoorTest, DoorInitiallyClosed) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(DoorTest, GetTimeoutDifferentValue) {
    TestTimedDoor anotherDoor(250);
    EXPECT_EQ(anotherDoor.getTimeOut(), 250);
}

TEST_F(DoorTest, MultipleLocksKeepDoorClosed) {
    door->lock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST(AdapterTest, AdapterDoesNotThrowWhenDoorClosed) {
    TimedDoor door(100);
    door.lock();
    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
}
