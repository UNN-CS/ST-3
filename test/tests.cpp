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
  EXPECT_EQ(timedDoor->getTimeOut(), 5);
}

TEST_F(TimedDoorTest, UnlockTest) {
  timedDoor->lock();

  try {
    timedDoor->unlock();
    EXPECT_TRUE(timedDoor->isDoorOpened());
  } catch (const std::runtime_error &) {
    EXPECT_TRUE(timedDoor->isDoorOpened());
  }
}

TEST_F(TimedDoorTest, LockTest) {
  timedDoor->lock();

  try {
    timedDoor->unlock();
    timedDoor->lock();
    EXPECT_FALSE(timedDoor->isDoorOpened());
  } catch (const std::runtime_error &) {
    timedDoor->lock();
    EXPECT_FALSE(timedDoor->isDoorOpened());
  }
}

TEST_F(TimedDoorTest, ThrowStateTest) {
  EXPECT_THROW(timedDoor->throwState(), std::runtime_error);
}

class DoorTimerAdapterTest : public ::testing::Test {
 protected:
  void SetUp() override { mockTimer = new MockTimer(); }

  void TearDown() override { delete mockTimer; }

  MockTimer *mockTimer;
};

TEST_F(DoorTimerAdapterTest, TimerRegisterTest) {
  EXPECT_CALL(*mockTimer, tregister(10, testing::_)).Times(1);

  mockTimer->tregister(10, nullptr);
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

  EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

TEST_F(TimerClientTest, TimeoutWithClosedDoorTest) {
  door->lock();
  EXPECT_NO_THROW(adapter->Timeout());
}

class MockDoorTest : public ::testing::Test {
 protected:
  void SetUp() override { mockDoor = new MockDoor(); }

  void TearDown() override { delete mockDoor; }

  MockDoor *mockDoor;
};

TEST_F(MockDoorTest, DoorMethodsSequenceTest) {
  EXPECT_CALL(*mockDoor, lock()).Times(1);
  EXPECT_CALL(*mockDoor, unlock()).Times(1);

  EXPECT_CALL(*mockDoor, isDoorOpened())
      .Times(1)
      .WillOnce(testing::Return(true));

  mockDoor->lock();
  mockDoor->unlock();

  EXPECT_TRUE(mockDoor->isDoorOpened());
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

class IntegrationTest : public ::testing::Test {
 protected:
  void SetUp() override { timedDoor = new TimedDoor(1); }

  void TearDown() override { delete timedDoor; }

  TimedDoor *timedDoor;
};

TEST_F(IntegrationTest, DoorTimeoutIntegrationTest) {
  timedDoor->lock();
  EXPECT_FALSE(timedDoor->isDoorOpened());

  timedDoor->unlock();
  EXPECT_TRUE(timedDoor->isDoorOpened());

  DoorTimerAdapter tempAdapter(*timedDoor);
  EXPECT_THROW(tempAdapter.Timeout(), std::runtime_error);

  timedDoor->lock();
  EXPECT_FALSE(timedDoor->isDoorOpened());
  EXPECT_NO_THROW(tempAdapter.Timeout());
}
