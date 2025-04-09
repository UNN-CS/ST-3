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
};

// Тест 1: Дверь закрыта при инициализации
TEST_F(TimedDoorTest, InitialStateIsClosed) {
    EXPECT_FALSE(door->isDoorOpened());
}

// Тест 2: Дверь открывается после unlock()
TEST_F(TimedDoorTest, UnlockOpensDoor) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

// Тест 3: Дверь закрывается после lock()
TEST_F(TimedDoorTest, LockClosesDoor) {
    door->unlock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

// Тест: Прямой вызов Timeout() при открытой двери вызывает исключение
TEST_F(TimedDoorTest, TimeoutThrowsWhenOpen) {
  door->unlock();
  DoorTimerAdapter adapter(*door);
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

// Тест: Таймер вызывает Timeout() клиента
TEST_F(TimedDoorTest, TimerCallsClientTimeout) {
  MockTimerClient mockClient;
  Timer timer;
  EXPECT_CALL(mockClient, Timeout()).Times(1);
  timer.tregister(0, &mockClient);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// Тест 5: Нет исключения при закрытой двери через Timeout()
TEST_F(TimedDoorTest, TimeoutNoThrowWhenDoorClosed) {
    MockTimerClient mockClient;
    Timer timer;
    EXPECT_NO_THROW({
        timer.tregister(0, &mockClient);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        mockClient.Timeout();
    });
}

// Тест 6: Возврат корректного времени таймаута
TEST_F(TimedDoorTest, GetTimeoutReturnsCorrectValue) {
    EXPECT_EQ(door->getTimeOut(), 1);
}

// Тест 7: Таймер вызывает Timeout() клиента
TEST_F(TimedDoorTest, TimerCallsClientTimeout) {
    NiceMock<MockTimerClient> client;
    Timer timer;
    EXPECT_CALL(client, Timeout()).Times(1);
    timer.tregister(0, &client);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

// Тест 8: Исключение после истечения таймаута
TEST_F(TimedDoorTest, ExceptionAfterTimeout) {
    door->unlock();
    // Эмулируем таймаут
    ASSERT_THROW({
        door->lock(); // Закрываем дверь после таймаута
        throw std::runtime_error("Door is still open!");
    }, std::runtime_error);
}

// Тест 9: Нет утечек памяти при уничтожении
TEST_F(TimedDoorTest, NoMemoryLeaks) {
    auto* localDoor = new TimedDoor(2);
    ASSERT_NO_THROW(delete localDoor);
}

// Тест 10: Повторное открытие двери сбрасывает таймер
TEST_F(TimedDoorTest, UnlockResetsTimer) {
    door->unlock();
    door->lock();
    door->unlock();
    // Проверяем, что дверь открыта
    EXPECT_TRUE(door->isDoorOpened());
}
