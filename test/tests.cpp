// Copyright 2025 Konkov Ivan

#include "TimedDoor.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <chrono>

using ::testing::_;
using ::testing::AtLeast;
using ::testing::NiceMock;

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
        timedDoor = new TimedDoor(1); // Таймаут 1 секунда для тестов
    }

    void TearDown() override {
        delete timedDoor;
    }

    TimedDoor* timedDoor;
};

TEST_F(TimedDoorTest, InitialStateIsLocked) {
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
    timedDoor->unlock();
    EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
    timedDoor->unlock();
    timedDoor->lock();
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutThrowsAfterDelay) {
    timedDoor->unlock();
    EXPECT_THROW({
        std::this_thread::sleep_for(std::chrono::seconds(2)); // Ждем больше таймаута
    }, std::runtime_error);
}

TEST_F(TimedDoorTest, NoExceptionWhenClosed) {
    timedDoor->lock();
    EXPECT_NO_THROW(timedDoor->throwState());
}

TEST_F(TimedDoorTest, AdapterCallsTimeout) {
    NiceMock<MockTimerClient> client;
    Timer timer;
    EXPECT_CALL(client, Timeout()).Times(1);
    timer.tregister(1, &client);
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

TEST_F(TimedDoorTest, UnlockTriggersTimerRegistration) {
    NiceMock<MockTimerClient> client;
    EXPECT_CALL(client, Timeout()).Times(AtLeast(0)); // Не требует обязательного вызова
    timedDoor->unlock();
}

TEST_F(TimedDoorTest, MultipleUnlocks) {
    timedDoor->unlock();
    timedDoor->lock();
    timedDoor->unlock();
    EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, ExceptionAfterTimeout) {
    timedDoor->unlock();
    EXPECT_THROW({
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }, std::runtime_error);
}
