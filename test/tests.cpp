// test/tests.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/TimedDoor.h"
#include <stdexcept>
#include <thread>
#include <chrono>

using ::testing::_;
using ::testing::Mock;
using ::testing::Return;

// Mock класс для TimerClient
class MockTimerClient : public TimerClient {
public:
    MOCK_METHOD(void, Timeout, (), (override));
};

// Mock класс для Door
class MockDoor : public Door {
public:
    MOCK_METHOD(void, lock, (), (override));
    MOCK_METHOD(void, unlock, (), (override));
    MOCK_METHOD(bool, isDoorOpened, (), (override));
};

// Тестовый фикстур для TimedDoor
class TimedDoorTest : public ::testing::Test {
protected:
    void SetUp() override {
        door = new TimedDoor(100);
    }
    
    void TearDown() override {
        delete door;
    }
    
    TimedDoor* door;
};

// Тест 1: Проверка создания объекта двери
TEST_F(TimedDoorTest, ConstructorInitializesCorrectly) {
    EXPECT_FALSE(door->isDoorOpened());
    EXPECT_EQ(door->getTimeOut(), 100);
}

// Тест 2: Проверка открытия двери
TEST_F(TimedDoorTest, UnlockOpensDoor) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

// Тест 3: Проверка закрытия двери
TEST_F(TimedDoorTest, LockClosesDoor) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

// Тест 4: Проверка выброса исключения при таймауте с открытой дверью
TEST_F(TimedDoorTest, TimeoutThrowsExceptionWhenDoorIsOpen) {
    door->unlock();
    DoorTimerAdapter adapter(*door);
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

// Тест 5: Проверка отсутствия исключения при таймауте с закрытой дверью
TEST_F(TimedDoorTest, TimeoutDoesNotThrowExceptionWhenDoorIsClosed) {
    door->lock();
    DoorTimerAdapter adapter(*door);
    EXPECT_NO_THROW(adapter.Timeout());
}

// Тест 6: Проверка работы адаптера
TEST(TimerAdapterTest, AdapterCallsThrowStateWhenDoorOpen) {
    TimedDoor door(100);
    door.unlock();
    DoorTimerAdapter adapter(door);
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

// Тест 7: Проверка, что адаптер не вызывает исключение при закрытой двери
TEST(TimerAdapterTest, AdapterDoesNotThrowWhenDoorClosed) {
    TimedDoor door(100);
    door.lock();
    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
}

// Тест 8: Проверка Timer с мок-клиентом
TEST(TimerTest, TimerCallsTimeoutOnClient) {
    MockTimerClient mockClient;
    Timer timer;
    
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    
    timer.tregister(10, &mockClient);
}

// Тест 9: Проверка, что Timer не вызывает Timeout при nullptr
TEST(TimerTest, TimerDoesNotCallWithNullClient) {
    Timer timer;
    timer.tregister(10, nullptr);
    SUCCEED();
}

// Тест 10: Проверка последовательности открытия и закрытия
TEST_F(TimedDoorTest, MultipleOpenCloseCycles) {
    for(int i = 0; i < 3; i++) {
        door->unlock();
        EXPECT_TRUE(door->isDoorOpened());
        door->lock();
        EXPECT_FALSE(door->isDoorOpened());
    }
}

// Тест 11: Проверка getTimeOut метода
TEST_F(TimedDoorTest, GetTimeOutReturnsCorrectValue) {
    EXPECT_EQ(door->getTimeOut(), 100);
}

// Тест 12: Проверка работы таймера с реальной задержкой
TEST(TimerIntegrationTest, TimerTriggersAfterTimeout) {
    TimedDoor door(50);
    door.unlock();
    Timer timer;
    
    EXPECT_THROW({
        timer.tregister(50, door.getAdapter());
    }, std::runtime_error);
}