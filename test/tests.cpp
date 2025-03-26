// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"

using ::testing::NiceMock;
using ::testing::Mock;
using ::testing::_;

// Мок-класс для тестирования таймера
class MockTimerClient : public TimerClient {
public:
    MOCK_METHOD(void, Timeout, (), (override));
};

// Тестирование открытия и закрытия двери
TEST(TimedDoorTest, DoorLockUnlock) {
    TimedDoor door(5);
    EXPECT_FALSE(door.isDoorOpened());
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

// Тестирование регистрации таймера
TEST(TimerTest, TimerTriggersTimeout) {
    NiceMock<MockTimerClient> mockClient;
    Timer timer;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    timer.tregister(1, &mockClient);
}

// Тестирование выброса исключения при открытой двери
TEST(TimedDoorTest, ThrowsExceptionIfLeftOpen) {
    TimedDoor door(1);
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

// Тестирование отсутствия исключения при закрытой двери
TEST(TimedDoorTest, NoExceptionIfClosed) {
    TimedDoor door(1);
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    door.lock();
    EXPECT_NO_THROW(door.throwState());
}
