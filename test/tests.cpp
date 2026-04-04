// Copyright 2021 GHA Test Team

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <thread>
#include "TimedDoor.h"

class MockableTimer {
 public:
  virtual ~MockableTimer() = default;
  virtual void tregister(int, TimerClient *) = 0;
};

class MockTimer : public MockableTimer {
 public:
  MOCK_METHOD(void, tregister, (int, TimerClient *), (override));
};

class MockDoorTimerAdapter : public TimerClient {
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
  void SetUp() override { timedDoor = new TimedDoor(5); }
  void TearDown() override { delete timedDoor; }

  TimedDoor *timedDoor;
};

TEST_F(TimedDoorTest, InitialStateTest) {
  EXPECT_FALSE(timedDoor->isDoorOpened());
  EXPECT_EQ(timedDoor->getTimeout(), 5);
}

TEST_F(TimedDoorTest, UnlockTest) {
  timedDoor->lock();
  timedDoor->unlock();
  EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, LockTest) {
  timedDoor->lock();
  timedDoor->unlock();
  timedDoor->lock();
  EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, ThrowStateTest) {
  timedDoor->unlock();
  EXPECT_THROW(timedDoor->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, MultipleUnlockTest) {
  timedDoor->unlock();
  timedDoor->unlock();
  EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, ZeroTimeoutTest) {
  EXPECT_THROW(TimedDoor zeroTimeoutDoor(0), std::invalid_argument);
}

TEST_F(TimedDoorTest, NegativeTimeoutTest) {
  EXPECT_THROW(TimedDoor negativeTimeoutDoor(-1), std::invalid_argument);
}

TEST_F(TimedDoorTest, LockOnClosedDoorTest) {
  timedDoor->lock();
  EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutWithClosedDoorTest) {
  timedDoor->lock();
  EXPECT_NO_THROW(timedDoor->throwState());
}

TEST_F(TimedDoorTest, MinimumTimeoutTest) {
  TimedDoor minTimeoutDoor(1);
  minTimeoutDoor.unlock();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  EXPECT_THROW(minTimeoutDoor.throwState(), std::runtime_error);
}

class DoorTimerAdapterTest : public ::testing::Test {
 protected:
  void SetUp() override { mockTimer = new MockTimer(); }
  void TearDown() override { delete mockTimer; }

  MockTimer *mockTimer;
};

TEST_F(DoorTimerAdapterTest, TimerRegisterTest) {
  EXPECT_CALL(*mockTimer, tregister(10, testing::_)).Times(1);
  if (mockTimer) {
    mockTimer->tregister(10, nullptr);
  }
}

class TimerClientTest : public ::testing::Test {
 protected:
  void SetUp() override {
    door = new TimedDoor(5);
    adapter = new DoorTimerAdapter(*door);
  }

  void TearDown() override {
    delete adapter;
    delete door;
  }

  TimedDoor *door;
  DoorTimerAdapter *adapter;
};

TEST_F(TimerClientTest, TimeoutWithOpenDoorTest) {
    door->lock();
    door->unlock();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

TEST_F(TimerClientTest, TimeoutWithClosedDoorTest) {
  door->lock();
  EXPECT_NO_THROW(adapter->Timeout());
}

class TimerAdapterInteractionTest : public ::testing::Test {
 protected:
  void SetUp() override { mockTimerClient = new MockDoorTimerAdapter(); }
  void TearDown() override { delete mockTimerClient; }

  MockDoorTimerAdapter *mockTimerClient;
};

TEST_F(TimerAdapterInteractionTest, TimerCallsTimeoutTest) {
  class TestTimer : public Timer {
   public:
    void tregister(int /*seconds*/, TimerClient *timerClient) {
      if (timerClient) {
        timerClient->Timeout();
      }
    }
  };

  EXPECT_CALL(*mockTimerClient, Timeout()).Times(1);

  TestTimer testTimer;
  testTimer.tregister(0, mockTimerClient);
}
