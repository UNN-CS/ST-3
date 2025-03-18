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
  void SetUp() override { door = new TimedDoor(5); }
  void TearDown() override { delete door; }
  TimedDoor *door;
};

TEST_F(TimedDoorTest, InitialState_test) {
  EXPECT_FALSE(door->isDoorOpened());
  EXPECT_EQ(door->getTimeOut(), 5);
}

TEST_F(TimedDoorTest, UnlockBehavior_test) {
  door->lock();
  EXPECT_NO_THROW(door->unlock());
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockBehavior_test) {
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, Exception_throwState_test) {
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, RepeatedUnlock_lock_switch_test) {
  door->lock();
  for (int i = 0; i < 3; ++i) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
  }
}

TEST_F(TimedDoorTest, MultipleLockCalls_test) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, MultipleUnlockCalls_test) {
  door->lock();
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, ThrowStateMultipleTimes_test) {
  EXPECT_THROW(door->throwState(), std::runtime_error);
  EXPECT_THROW(door->throwState(), std::runtime_error);
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

class DoorTimerAdapterTest : public ::testing::Test {
 protected:
  void SetUp() override { timer = new MockTimer(); }
  void TearDown() override { delete timer; }
  MockTimer *timer;
};

TEST_F(DoorTimerAdapterTest, TimerRegister_test) {
  EXPECT_CALL(*timer, tregister(10, testing::_)).Times(1);
  timer->tregister(10, nullptr);
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

TEST_F(TimerClientTest, TimerAdapterTimeout_openDoor_test) {
  door->unlock();
  EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

TEST_F(TimerClientTest, TimerAdapterTimeout_closedDoor_test) {
  door->lock();
  EXPECT_NO_THROW(adapter->Timeout());
}

class MockDoorTest : public ::testing::Test {
 protected:
  void SetUp() override { mockDoor = new MockDoor(); }
  void TearDown() override { delete mockDoor; }
  MockDoor *mockDoor;
};

TEST_F(MockDoorTest, DoorMethodSequence_test) {
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

TEST_F(TimerAdapterInteractionTest, TimerCallsTimeout_test) {
  class TestTimer : public Timer {
   public:
    void tregister(int, TimerClient *client) {
      if (client) {
        client->Timeout();
      }
    }
  };
  EXPECT_CALL(*mockTimerClient, Timeout()).Times(1);
  TestTimer testTimer;
  testTimer.tregister(0, mockTimerClient);
}

class IntegrationTest : public ::testing::Test {
 protected:
  void SetUp() override { door = new TimedDoor(1); }
  void TearDown() override { delete door; }
  TimedDoor *door;
};

TEST_F(IntegrationTest, Integration_DoorTimeout_test) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  DoorTimerAdapter adapter(*door);
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
  EXPECT_NO_THROW(adapter.Timeout());
}

TEST_F(IntegrationTest, DoorReopenBeforeTimeout_test) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
  DoorTimerAdapter adapter(*door);
  EXPECT_NO_THROW(adapter.Timeout());
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}
