// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include "TimedDoor.h"
#include <stdexcept>

using ::testing::_;
using ::testing::StrictMock;

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
  }

  void TearDown() override {
    delete timedDoor;
  }

  TimedDoor* timedDoor;
};

TEST_F(TimedDoorTest, InitialStateTest) {
  EXPECT_FALSE(timedDoor->isDoorOpened());
  EXPECT_EQ(timedDoor->getTimeOut(), 1000);
}

TEST_F(TimedDoorTest, UnlockTest) {
  timedDoor->unlock();
  EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, LockTest) {
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
  void SetUp() override {
    timedDoor = new TimedDoor(1000);
    adapter = new DoorTimerAdapter(*timedDoor);
  }

  void TearDown() override {
    delete adapter;
    delete timedDoor;
  }

  TimedDoor* timedDoor;
  DoorTimerAdapter* adapter;
};

TEST_F(DoorTimerAdapterTest, TimerRegisterTest) {
  StrictMock<MockTimerClient> mockClient;
  EXPECT_CALL(mockClient, Timeout()).Times(1);

  Timer timer;
  timer.tregister(100, &mockClient);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

class TimerClientTest : public ::testing::Test {
 protected:
  void SetUp() override {
    timedDoor = new TimedDoor(1000);
    adapter = new DoorTimerAdapter(*timedDoor);
  }

  void TearDown() override {
    delete adapter;
    delete timedDoor;
  }

  TimedDoor* timedDoor;
  DoorTimerAdapter* adapter;
};

TEST_F(TimerClientTest, TimeoutWithOpenDoorTest) {
  timedDoor->unlock();
  EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

TEST_F(TimerClientTest, TimeoutWithClosedDoorTest) {
  timedDoor->lock();
  EXPECT_NO_THROW(adapter->Timeout());
}

class TimerAdapterInteractionTest : public ::testing::Test {
 protected:
  void SetUp() override {
    mockTimerClient = new MockTimerClient();
  }

  void TearDown() override {
    delete mockTimerClient;
  }

  MockTimerClient* mockTimerClient;
};

TEST_F(TimerAdapterInteractionTest, TimerCallsTimeoutTest) {
  EXPECT_CALL(*mockTimerClient, Timeout()).Times(1);

  Timer timer;
  timer.tregister(100, mockTimerClient);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
}
