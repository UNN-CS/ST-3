// Copyright 2025 Konkov Ivan

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "TimedDoor.h"
#include <thread>

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;

// Mock-класс для таймера
class MockTimerClient : public TimerClient {
public:
    MOCK_METHOD(void, Timeout, (), (override));
};

// Mock-класс для двери
class MockDoor : public Door {
public:
    MOCK_METHOD(void, lock, (), (override));
    MOCK_METHOD(void, unlock, (), (override));
    MOCK_METHOD(bool, isDoorOpened, (), (override));
};

// Тест 1: Проверка блокировки двери
TEST(TimedDoorTest, LockDoor) {
    TimedDoor door(5);
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

// Тест 2: Проверка разблокировки двери
TEST(TimedDoorTest, UnlockDoor) {
    TimedDoor door(5);
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

// Тест 3: Проверка таймаута
TEST(TimedDoorTest, ThrowExceptionWhenOpenedTooLong) {
    TimedDoor door(1);
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

// Тест 4: Проверка отсутствия исключения при закрытой двери
TEST(TimedDoorTest, NoExceptionWhenClosed) {
    TimedDoor door(1);
    door.lock();
    EXPECT_NO_THROW(door.throwState());
}

// Тест 5: Проверка вызова Timeout()
TEST(TimedDoorTest, TimerTriggersTimeout) {
    MockTimerClient mockTimer;
    EXPECT_CALL(mockTimer, Timeout()).Times(1);
    mockTimer.Timeout();
}

// Тест 6: Проверка состояния двери после закрытия
TEST(TimedDoorTest, CheckDoorStateAfterLock) {
    TimedDoor door(3);
    door.unlock();
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

// Тест 7: Проверка адаптера
TEST(TimedDoorTest, DoorAdapterTriggersTimeout) {
    TimedDoor door(2);
    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
}

// Тест 8: Проверка вызова unlock()
TEST(TimedDoorTest, UnlockCallsChangeState) {
    MockDoor mockDoor;
    EXPECT_CALL(mockDoor, unlock()).Times(1);
    mockDoor.unlock();
}

// Тест 9: Проверка вызова lock()
TEST(TimedDoorTest, LockCallsChangeState) {
    MockDoor mockDoor;
    EXPECT_CALL(mockDoor, lock()).Times(1);
    mockDoor.lock();
}

// Тест 10: Проверка вызова isDoorOpened()
TEST(TimedDoorTest, CheckIsDoorOpenedMock) {
    MockDoor mockDoor;
    EXPECT_CALL(mockDoor, isDoorOpened()).WillOnce(Return(true));
    EXPECT_TRUE(mockDoor.isDoorOpened());
}
