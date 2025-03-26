// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"
#include <cstdint>
#include <chrono>
#include <thread>
// Copyright 2021 GHA Test Team

class TimedDoorFixture : public ::testing::Test {
protected:
    TimedDoor* door;
    void SetUp() override {
        door = new TimedDoor(1);
    }
    void TearDown() override {
        // Если дверь осталась открытой, пытаемся дождаться завершения таймера
        try {
            door->joinTimerThread();
        }
        catch (...) {
            // Игнорируем исключение в TearDown
        }
        delete door;
    }
};

// Тест 1: Проверка, что после вызова lock() дверь закрыта.
TEST_F(TimedDoorFixture, DoorIsClosedAfterLock) {
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

// Тест 2: Проверка, что после unlock() дверь открыта (до срабатывания таймера).
TEST_F(TimedDoorFixture, DoorIsOpenedAfterUnlock) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    EXPECT_THROW(door->joinTimerThread(), std::runtime_error);
}

// Тест 3: Проверка выброса исключения при открытой двери после таймаута.
TEST_F(TimedDoorFixture, ExceptionThrownWhenDoorRemainsOpen) {
    door->unlock();
    EXPECT_THROW(door->joinTimerThread(), std::runtime_error);
}

// Тест 4: Проверка отсутствия исключения, если дверь закрывается до истечения таймаута.
TEST_F(TimedDoorFixture, NoExceptionWhenDoorClosedBeforeTimeout) {
    TimedDoor doorLocal(2);
    doorLocal.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    doorLocal.lock();
    EXPECT_NO_THROW(doorLocal.joinTimerThread());
}

// Тест 5: Проверка возвращаемого значения таймаута.
TEST_F(TimedDoorFixture, GetTimeOutReturnsCorrectValue) {
    int timeout = 3;
    TimedDoor doorLocal(timeout);
    EXPECT_EQ(doorLocal.getTimeOut(), timeout);
}

// Тест 6: Использование mock для проверки вызова метода Timeout адаптера.
// Создадим фиктивный класс-наследник TimerClient.
class MockTimerClient : public TimerClient {
public:
    MOCK_METHOD(void, Timeout, (), (override));
};

TEST(TimedDoorTests, AdapterTimeoutIsCalled) {
    // Создаем mock-объект
    MockTimerClient mockClient;
    // Ожидаем, что метод Timeout будет вызван ровно 1 раз.
    EXPECT_CALL(mockClient, Timeout()).Times(1);

    Timer timer;
    timer.tregister(0, &mockClient);  // таймаут 0 для мгновенного вызова
}

// Тест 7: Проверка работы lock() – дверь становится закрытой.
TEST_F(TimedDoorFixture, LockClosesDoor) {
    door->unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

// Тест 8: Проверка работы unlock() – дверь становится открытой.
TEST_F(TimedDoorFixture, UnlockOpensDoor) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    EXPECT_THROW(door->joinTimerThread(), std::runtime_error);
}

// Тест 9: Проверка, что joinTimerThread() корректно завершает поток таймера.
TEST_F(TimedDoorFixture, JoinTimerThreadWorksCorrectly) {
    door->unlock();
    EXPECT_THROW(door->joinTimerThread(), std::runtime_error);
}

// Тест 10: Проверка, что повторный вызов joinTimerThread() (если поток уже завершён) не приводит к ошибке.
TEST_F(TimedDoorFixture, MultipleJoinTimerThreadCallsDoNotFail) {
    door->unlock();
    try {
        door->joinTimerThread();
    }
    catch (...) {
        // Игнорируем исключение первого вызова
    }
    // Повторный вызов не должен приводить к сбою.
    EXPECT_NO_THROW(door->joinTimerThread());
}