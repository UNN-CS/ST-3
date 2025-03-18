// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"
#include <thread>





class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
    TimedDoor* door;
    MockTimerClient* mockClient;
    
    void SetUp() override {
        door = new TimedDoor(5);
        mockClient = new MockTimerClient();
    }
    
    void TearDown() override {
        delete door;
        delete mockClient;
    }
};

TEST_F(TimedDoorTest, TestTimeoutAfterOpening) {
    door->unlock();
    
    EXPECT_CALL(*mockClient, Timeout())
        .Times(1);
    
    Timer timer;
    timer.tregister(5, mockClient);
}

TEST_F(TimedDoorTest, TestThrowExceptionIfDoorNotOpened) {
    EXPECT_THROW(door->throwState(), std::runtime_error);
}
TEST_F(TimedDoorTest, TestDoorInitialState) {
    EXPECT_FALSE(door->isDoorOpened());  // Дверь должна быть закрыта по умолчанию
  }
  
  TEST_F(TimedDoorTest, TestUnlockDoor) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());  // Дверь должна быть открыта после вызова unlock
  }
  
  TEST_F(TimedDoorTest, TestLockDoor) {
    door->unlock();  // Сначала откроем дверь
    door->lock();    // Затем закроем её
    EXPECT_FALSE(door->isDoorOpened());  // Дверь должна быть закрыта после вызова lock
  }
  
  TEST_F(TimedDoorTest, TestTimerStartAfterUnlock) {
    door->unlock();  // Открытие двери вызывает запуск таймера
  
    // Используй EXPECT_CALL до выполнения таймера
    EXPECT_CALL(*mockClient, Timeout()).Times(1);
  
    Timer timer;
    timer.tregister(5, mockClient);
  
    // Явно ждем, чтобы убедиться, что таймер успевает сработать
    std::this_thread::sleep_for(std::chrono::seconds(6));  // Ждем больше времени, чтобы таймер успел сработать
  }
  
  TEST_F(TimedDoorTest, TestTimerRegistration) {
    EXPECT_CALL(*mockClient, Timeout()).Times(1);
    Timer timer;
    timer.tregister(5, mockClient);
  }
  
  TEST_F(TimedDoorTest, TestLockDoorAfterTimeout) {
    door->unlock();
    std::this_thread::sleep_for(std::chrono::seconds(6));  // Даем время для таймера
    door->lock();  // Дверь должна быть закрыта
    EXPECT_FALSE(door->isDoorOpened());
  }
  
  TEST_F(TimedDoorTest, TestCorrectTimeout) {
    door->unlock();
    EXPECT_EQ(door->getTimeOut(), 5);  // Проверяем, что время таймера установлено верно
  }
  
  TEST_F(TimedDoorTest, TestMultipleLockUnlock) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
  }