// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
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
    auto mockClient = std::make_shared<NiceMock<MockTimerClient>>();
    
    std::promise<void> timeoutPromise;
    auto timeoutFuture = timeoutPromise.get_future();
    
    EXPECT_CALL(*mockClient, Timeout()).WillOnce([&timeoutPromise]() {
        timeoutPromise.set_value();
    });
    
    Timer timer;
    timer.tregister(1, mockClient.get());
    
    ASSERT_EQ(timeoutFuture.wait_for(std::chrono::seconds(2)), std::future_status::ready);
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
