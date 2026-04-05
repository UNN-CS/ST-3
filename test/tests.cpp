// Copyright 2021 GHA Test Team

#include "../include/TimedDoor.h"
#include <chrono>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdexcept>
#include <thread>

using ::testing::_;
using ::testing::Mock;

class MockTimerClient : public TimerClient {
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
  void SetUp() override {
    door_ = new TimedDoor(1000);
  }

  void TearDown() override {
    delete door_;
  }

  TimedDoor* door_;
};

class DoorTimerAdapterTest : public ::testing::Test {
 protected:
  void SetUp() override {
    door_ = new TimedDoor(1000);
    adapter_ = new DoorTimerAdapter(door_);
  }

  void TearDown() override {
    delete adapter_;
    delete door_;
  }

  TimedDoor* door_;
  DoorTimerAdapter* adapter_;
};

TEST_F(TimedDoorTest, TimedDoorCreation) {
  EXPECT_FALSE(door_->isDoorOpened());
  EXPECT_EQ(door_->getTimeOut(), 1000);
}

TEST_F(TimedDoorTest, DoorUnlockOpensDoor) {
  door_->unlock();
  EXPECT_TRUE(door_->isDoorOpened());
  door_->lock();
}

TEST_F(TimedDoorTest, DoorLockClosesDoor) {
  door_->unlock();
  EXPECT_TRUE(door_->isDoorOpened());
  door_->lock();
  EXPECT_FALSE(door_->isDoorOpened());
}

TEST_F(TimedDoorTest, ThrowStateThrowsException) {
  door_->unlock();
  EXPECT_THROW(door_->throwState(), std::runtime_error);
  door_->lock();
}

TEST_F(TimedDoorTest, ThrowStateNoExceptionWhenClosed) {
  door_->lock();
  EXPECT_NO_THROW(door_->throwState());
}

TEST_F(DoorTimerAdapterTest, AdapterTimeoutCanBeCalled) {
  EXPECT_NO_THROW(adapter_->Timeout());
}

TEST_F(TimedDoorTest, DoorRemainsOpenAfterUnlock) {
  door_->unlock();
  EXPECT_TRUE(door_->isDoorOpened());
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  EXPECT_TRUE(door_->isDoorOpened());
  door_->lock();
}

TEST_F(TimedDoorTest, DoorRemainsClosedAfterLock) {
  door_->lock();
  EXPECT_FALSE(door_->isDoorOpened());
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  EXPECT_FALSE(door_->isDoorOpened());
}

TEST_F(TimedDoorTest, MultipleUnlockLockCycles) {
  for (int i = 0; i < 3; ++i) {
    door_->unlock();
    EXPECT_TRUE(door_->isDoorOpened());
    door_->lock();
    EXPECT_FALSE(door_->isDoorOpened());
  }
}

TEST_F(TimedDoorTest, GetTimeOutReturnsCorrectValue) {
  EXPECT_EQ(door_->getTimeOut(), 1000);
  TimedDoor door2(500);
  EXPECT_EQ(door2.getTimeOut(), 500);
}

TEST_F(TimedDoorTest, DoubleUnlockHasNoSideEffects) {
  door_->unlock();
  EXPECT_TRUE(door_->isDoorOpened());
  door_->unlock();
  EXPECT_TRUE(door_->isDoorOpened());
  door_->lock();
}

TEST_F(TimedDoorTest, LockingClosedDoorDoesNothing) {
  door_->lock();
  EXPECT_FALSE(door_->isDoorOpened());
  door_->lock();
  EXPECT_FALSE(door_->isDoorOpened());
}

TEST(TimedDoorTestSuite, DifferentTimeoutValues) {
  TimedDoor door1(500);
  EXPECT_EQ(door1.getTimeOut(), 500);

  TimedDoor door2(1500);
  EXPECT_EQ(door2.getTimeOut(), 1500);

  TimedDoor door3(0);
  EXPECT_EQ(door3.getTimeOut(), 0);
}

TEST(TimerClientTest, TimerClientCanBeImplemented) {
  class TestClient : public TimerClient {
   public:
    bool timeoutCalled = false;
    void Timeout() override { timeoutCalled = true; }
  };

  TestClient client;
  EXPECT_FALSE(client.timeoutCalled);
  client.Timeout();
  EXPECT_TRUE(client.timeoutCalled);
}

TEST(DoorTest, DoorInterfaceCanBeImplemented) {
  class TestDoor : public Door {
   public:
    bool opened = false;
    void lock() override { opened = false; }
    void unlock() override { opened = true; }
    bool isDoorOpened() override { return opened; }
  };

  TestDoor testDoor;
  testDoor.unlock();
  EXPECT_TRUE(testDoor.isDoorOpened());
  testDoor.lock();
  EXPECT_FALSE(testDoor.isDoorOpened());
}

TEST_F(DoorTimerAdapterTest, AdapterConstruction) {
  TimedDoor testDoor(1000);
  DoorTimerAdapter* testAdapter = new DoorTimerAdapter(&testDoor);
  EXPECT_NO_THROW(delete testAdapter);
}

TEST_F(TimedDoorTest, ConcurrentUnlockLock) {
  std::thread t1([this]() { door_->unlock(); });
  std::thread t2([this]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    door_->lock();
  });
  t1.join();
  t2.join();
  SUCCEED();
}

TEST_F(TimedDoorTest, DestructorCleansUp) {
  TimedDoor* testDoor = new TimedDoor(1000);
  EXPECT_NO_THROW(delete testDoor);
}

TEST_F(TimedDoorTest, GetTimeOutAfterOperations) {
  int initialTimeout = door_->getTimeOut();
  door_->unlock();
  EXPECT_EQ(door_->getTimeOut(), initialTimeout);
  door_->lock();
  EXPECT_EQ(door_->getTimeOut(), initialTimeout);
}

TEST_F(TimedDoorTest, NoExceptionWhenDoorClosedQuickly) {
  EXPECT_NO_THROW({
    door_->unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    door_->lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(900));
  });
}

TEST_F(TimedDoorTest, TimerCancelledWhenDoorLocked) {
  EXPECT_NO_THROW({
    door_->unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    door_->lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(900));
  });
}
