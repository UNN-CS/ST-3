// Copyright 2025 Mikhail Burykin

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "TimedDoor.h"
#include <thread>
#include <chrono>
#include <stdexcept>

static bool exceptionThrown = false;

class TestDoorTimerAdapter : public DoorTimerAdapter {
private:
    TimedDoor& door;
public:
    explicit TestDoorTimerAdapter(TimedDoor& d) : DoorTimerAdapter(d), door(d) {}
    void Timeout() override {
        if (door.isDoorOpened()) {
            exceptionThrown = true;
            throw std::runtime_error("Door is still open!");
        }
    }
};

class MockTimerClient : public TimerClient {
public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
protected:
    TimedDoor* door{};

    void SetUp() override {
        exceptionThrown = false;
        door = new TimedDoor(1);
        door->setAdapter(new TestDoorTimerAdapter(*door));  // Заменяем адаптер
    }

    void TearDown() override {
        delete door;
    }
};

TEST_F(TimedDoorTest, DoorInitiallyClosed) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorOpensOnUnlock) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorClosesOnLock) {
    door->unlock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeoutReturnsCorrectValue) {
    EXPECT_EQ(door->getTimeOut(), 1);
}

TEST_F(TimedDoorTest, ExceptionThrownWhenDoorOpenAfterTimeout) {
    door->unlock();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_TRUE(exceptionThrown);
}

TEST_F(TimedDoorTest, NoExceptionWhenDoorClosedBeforeTimeout) {
    door->unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    door->lock();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_FALSE(exceptionThrown);
}

TEST_F(TimedDoorTest, ExceptionThrownOnReopenAfterClose) {
    door->unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    door->lock();
    door->unlock();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_TRUE(exceptionThrown);
}

TEST_F(TimedDoorTest, TimeoutCalledWhenTimerExpires) {
    MockTimerClient mockClient;
    Timer timer;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    timer.tregister(1, &mockClient);
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

TEST_F(TimedDoorTest, NoExceptionWithoutUnlock) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_FALSE(exceptionThrown);
}

TEST_F(TimedDoorTest, NoExceptionOnMultipleOpenClose) {
    door->unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    door->lock();
    door->unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    door->lock();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_FALSE(exceptionThrown);
}
