// Copyright 2021 GHA Test Team
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include "TimedDoor.h"
#include <stdexcept>

using ::testing::_;

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD0(Timeout, void());
};

class MockDoor : public Door {
 public:
  MOCK_METHOD0(lock, void());
  MOCK_METHOD0(unlock, void());
  MOCK_CONST_METHOD0(isDoorOpened, bool());
};

class TimedDoorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    timedDoor = new TimedDoor(1000);
    mockDoor = new MockDoor();
  }

  void TearDown() override {
    delete timedDoor;
    delete mockDoor;
  }

  TimedDoor* timedDoor;
  MockDoor* mockDoor;
};

TEST_F(TimedDoorTest, DoorStartsClosed) {
  EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorUnlocks) {
  timedDoor->unlock();
  EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorLocks) {
  timedDoor->unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  timedDoor->lock();
  EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutThrowsWhenDoorOpened) {
  timedDoor->unlock();
  EXPECT_THROW({
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
  }, std::runtime_error);
}

TEST_F(TimedDoorTest, TimeoutDoesNotThrowWhenDoorClosed) {
  timedDoor->unlock();
  timedDoor->lock();
  EXPECT_NO_THROW({
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
  });
}

TEST_F(TimedDoorTest, AdapterCallsTimeout) {
  MockTimerClient mockClient;
  EXPECT_CALL(mockClient, Timeout()).Times(1);

  Timer timer;
  timer.tregister(100, &mockClient);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

TEST_F(TimedDoorTest, TimerRegistersClient) {
  MockTimerClient mockClient;
  Timer timer;

  EXPECT_CALL(mockClient, Timeout()).Times(1);
  timer.tregister(100, &mockClient);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

TEST_F(TimedDoorTest, DoorUnlockActivatesTimer) {
  testing::NiceMock<MockTimerClient> mockClient;
  Timer timer;

  EXPECT_CALL(mockClient, Timeout()).Times(1);
  timedDoor->unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(1100));
}

TEST_F(TimedDoorTest, DoorLockDeactivatesTimer) {
  testing::NiceMock<MockTimerClient> mockClient;
  Timer timer;

  EXPECT_CALL(mockClient, Timeout()).Times(0);
  timedDoor->unlock();
  timedDoor->lock();
  std::this_thread::sleep_for(std::chrono::milliseconds(1100));
}

TEST_F(TimedDoorTest, DoorTimeoutValueIsCorrect) {
  EXPECT_EQ(timedDoor->getTimeOut(), 1000);
}
