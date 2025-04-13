// Copyright 2021 GHA Test Team

#include "../include/TimedDoor.h"
#include <gmock/gmock.h>
#include <thread>


class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class Test : public ::testing::Test {
 protected:
  TimedDoor *door;
  MockTimerClient *mockClient;

  void SetUp() override {
    door = new TimedDoor(50);
    mockClient = new MockTimerClient();
  }

  void TearDown() override {
    delete door;
    delete mockClient;
  }
};

TEST_F(Test, InitialState) { EXPECT_FALSE(door->isDoorOpened()); }

TEST_F(Test, OpenDoor) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
} 

TEST_F(Test, CloseDoor) {
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(Test, ExceptionOnTimeout) {
  door->unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(150));
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(Test, NoExceptionWhileClosed) {
  door->unlock();
  door->lock();
  std::this_thread::sleep_for(std::chrono::milliseconds(150));
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(Test, TimerResetOnReopen) {
    door->unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
    door->lock();

    door->unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(3));

    door->lock();

    // Проверяем что состояние корректно
    EXPECT_FALSE(door->isDoorOpened());
    EXPECT_NO_THROW(door->throwState());
}

TEST_F(Test, AdapterTriggerTimeout) {
  DoorTimerAdapter adapter(*door);
  door->unlock();
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(Test, GetTimeoutVal) { EXPECT_EQ(door->getTimeOut(), 50); }

TEST_F(Test, ReopenAfterClose) {
  door->unlock();
  door->lock();
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(Test, CloseAgain) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}
