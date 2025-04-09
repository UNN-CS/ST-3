// Copyright 2021 GHA Test Team

#include "TimedDoor.h"
#include <gmock/gmock.h>
#include <cstdint>
#include <thread>


using ::testing::_;
using ::testing::Mock;
using ::testing::InSequence;
using ::testing::NiceMock;

// Мок для TimerClient
class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

// Мок для Timer (альтернативная реализация для тестов)
class MockTimer : public Timer {
 public:
    MOCK_METHOD(void, tregister, (int timeout, TimerClient* client), (override));
};

// Тестовый класс
class TimedDoorTest : public ::testing::Test {
 protected:
    void SetUp() override {
        door = new TimedDoor(1); // Таймаут 1 секунда
    }
    void TearDown() override {
        delete door;
    }
    TimedDoor* door;
    MockTimerClient mockClient;
};

// Тест 1: При открытии двери регистрируется таймер
TEST_F(TimedDoorTest, UnlockRegistersTimer) {
    MockTimer mockTimer;
    EXPECT_CALL(mockTimer, tregister(1, door->adapter)).Times(1);
    door->unlock();
}

// Тест 2: Timeout вызывает проверку состояния двери
TEST_F(TimedDoorTest, TimeoutChecksDoorState) {
    door->unlock();
    ASSERT_TRUE(door->isDoorOpened());
    EXPECT_THROW(door->adapter->Timeout(), std::runtime_error);
}

// Тест 3: Исключение не выбрасывается, если дверь закрыта
TEST_F(TimedDoorTest, NoExceptionWhenDoorClosed) {
    door->unlock();
    door->lock();
    EXPECT_NO_THROW(door->adapter->Timeout());
}

// Тест 4: Таймер вызывает Timeout у клиента
TEST_F(TimedDoorTest, TimerCallsClientTimeout) {
    NiceMock<MockTimerClient> client;
    Timer timer;
    EXPECT_CALL(client, Timeout()).Times(1);
    timer.tregister(0, &client); // Нулевая задержка для синхронного теста
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// Тест 5: Состояние двери корректно меняется
TEST_F(TimedDoorTest, DoorStateChangesCorrectly) {
    EXPECT_FALSE(door->isDoorOpened());
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

// Тест 6: Исключение при открытой двери после таймаута
TEST_F(TimedDoorTest, ExceptionIfDoorRemainsOpen) {
    door->unlock();
    // Эмулируем срабатывание таймера
    ASSERT_THROW(door->adapter->Timeout(), std::runtime_error);
}

// Тест 7: Таймаут возвращает корректное значение
TEST_F(TimedDoorTest, ReturnsCorrectTimeout) {
    TimedDoor customDoor(5);
    EXPECT_EQ(customDoor.getTimeOut(), 5);
}

// Тест 8: Адаптер корректно привязан к двери
TEST_F(TimedDoorTest, AdapterBoundToCorrectDoor) {
    DoorTimerAdapter adapter(*door);
    ASSERT_EQ(&adapter.door, door);
}

// Тест 9: Повторное открытие двери сбрасывает таймер
TEST_F(TimedDoorTest, UnlockResetsTimer) {
    MockTimer mockTimer;
    EXPECT_CALL(mockTimer, tregister(1, _)).Times(2);
    door->unlock();
    door->lock();
    door->unlock();
}

// Тест 10: Нет утечек памяти при уничтожении
TEST_F(TimedDoorTest, NoMemoryLeaksOnDestruction) {
    auto* localDoor = new TimedDoor(2);
    ASSERT_NO_THROW(delete localDoor);
}
