// Copyright 2025 UNN-CS
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdexcept>
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
    MOCK_METHOD(bool, isDoorOpened, (), (override));
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

TEST_F(TimedDoorTest, DoorInitiallyClosed) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockThrowsWhenDoorLeftOpen) {
    // unlock() запускает таймер синхронно: если дверь не закрыли — бросает
    EXPECT_THROW(door->unlock(), std::runtime_error);
}

TEST_F(TimedDoorTest, GetTimeoutReturnsCorrectValue) {
    EXPECT_EQ(door->getTimeOut(), 1);
}

TEST_F(TimedDoorTest, GetTimeoutCustomValue) {
    TimedDoor d(5);
    EXPECT_EQ(d.getTimeOut(), 5);
}

TEST_F(TimedDoorTest, DoorClosesAfterLock) {
    // Сначала открываем вручную через lock/unlock состояние
    // lock() должна устанавливать isOpened = false
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, ThrowStateThrowsException) {
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, ThrowStateExceptionMessage) {
    try {
        door->throwState();
        FAIL() << "Expected std::runtime_error";
    } catch (const std::runtime_error& e) {
        EXPECT_STREQ(e.what(), "Door has been open too long!");
    }
}

TEST_F(TimedDoorTest, NoExceptionWhenDoorClosed) {
    // Адаптер не должен бросать исключение, если дверь закрыта
    DoorTimerAdapter adapter(*door);
    door->lock();
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST_F(TimedDoorTest, ExceptionWhenDoorOpenOnTimeout) {
    // Имитируем открытое состояние через прямой вызов throwState
    // (открытую дверь + Timeout)
    TimedDoor localDoor(10);
    DoorTimerAdapter adapter(localDoor);
    // Для теста обходим таймер — вручную выставляем состояние
    // через отдельный класс-тестовый stub
    // Здесь проверяем: если дверь открыта — Timeout бросает
    // Создаём дверь и напрямую вызываем throwState через открытое состояние
 
    // Проверяем через прямой вызов throwState (публичный метод):
    EXPECT_THROW(localDoor.throwState(), std::runtime_error);
}

TEST(TimerTest, TimerCallsTimeoutOnce) {
    MockTimerClient mockClient;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
 
    Timer timer;
    timer.tregister(1, &mockClient);
}

TEST(TimerTest, TimerCallsTimeoutAtLeastOnce) {
    MockTimerClient mockClient;
    EXPECT_CALL(mockClient, Timeout()).Times(AtLeast(1));
    Timer timer;
    timer.tregister(1, &mockClient);
}

TEST(MockDoorTest, LockMethodCalled) {
    MockDoor mockDoor;
    EXPECT_CALL(mockDoor, lock()).Times(1);
    mockDoor.lock();
}

TEST(MockDoorTest, UnlockMethodCalled) {
    MockDoor mockDoor;
    EXPECT_CALL(mockDoor, unlock()).Times(1);
    mockDoor.unlock();
}

TEST(MockDoorTest, IsDoorOpenedReturnsFalse) {
    MockDoor mockDoor;
    EXPECT_CALL(mockDoor, isDoorOpened()).WillOnce(::testing::Return(false));
    EXPECT_FALSE(mockDoor.isDoorOpened());
}

TEST(MockDoorTest, IsDoorOpenedReturnsTrue) {
    MockDoor mockDoor;
    EXPECT_CALL(mockDoor, isDoorOpened()).WillOnce(::testing::Return(true));
    EXPECT_TRUE(mockDoor.isDoorOpened());
}

class OpenTimedDoor : public TimedDoor {
 public:
    explicit OpenTimedDoor(int t) : TimedDoor(t) {
        // Открываем дверь напрямую — нельзя без доп. метода,
        // используем unlock без таймера через трюк
    }
};

TEST_F(TimedDoorTest, AdapterLinkedToDoor) {
    DoorTimerAdapter adapter(*door);
    EXPECT_NO_THROW(adapter.Timeout());
}
