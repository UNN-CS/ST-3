// Copyright 2024 SecureDoor Team

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <memory>
#include "TimedDoor.h"

using ::testing::_;
using ::testing::AtLeast;

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door {
 public:
    MOCK_METHOD(void, lock, (), (override));
    MOCK_METHOD(void, unlock, (), (override));
    MOCK_METHOD(bool, isDoorOpened, (), (const, override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
    void SetUp() override {
        door = new TimedDoor(3);
    }
    void TearDown() override {
        delete door;
    }
    TimedDoor* door;
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

TEST_F(TimedDoorTest, ThrowStateWhenOpen) {
    door->unlock();
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, ValidTimeoutCreation) {
    EXPECT_NO_THROW(TimedDoor validDoor(5));
}

TEST_F(TimedDoorTest, InvalidTimeoutThrows) {
    EXPECT_THROW(TimedDoor invalidDoor(0), std::invalid_argument);
    EXPECT_THROW(TimedDoor invalidDoor(-1), std::invalid_argument);
}

TEST_F(TimedDoorTest, MultipleUnlockCalls) {
    door->unlock();
    door->unlock(); // Должно оставаться открытым
    EXPECT_TRUE(door->isDoorOpened());
}

TEST(AdapterTest, AdapterCallsThrowState) {
    TimedDoor door(2);
    DoorTimerAdapter adapter(door);
    door.unlock();
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(TimerTest, TimerRegistration) {
    auto client = std::make_shared<MockTimerClient>();
    EXPECT_CALL(*client, Timeout()).Times(1);

    Timer timer;
    timer.tregister(0, client.get());

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(IntegrationTest, FullTimeoutCycle) {
    TimedDoor door(1);
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_THROW(door.throwState(), std::runtime_error);
}
