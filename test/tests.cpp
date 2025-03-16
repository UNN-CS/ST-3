// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <thread>
#include "../include/TimedDoor.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Invoke;

class MockTimerClient : public TimerClient {
public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class MockTimer : public Timer {
public:
    MOCK_METHOD(void, tregister, (int, TimerClient*), (override));
};

class TimedDoorTest : public ::testing::Test {
protected:
    void SetUp() override {
        timedDoor = new TimedDoor(100);
    }

    void TearDown() override {
        delete timedDoor;
    }

    TimedDoor* timedDoor;
};

TEST_F(TimedDoorTest, DoorInitialStateIsClosed) {
    ASSERT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensTheDoor) {
    timedDoor->unlock();
    ASSERT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesTheDoor) {
    timedDoor->unlock();
    timedDoor->lock();
    ASSERT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeout) {
    ASSERT_EQ(timedDoor->getTimeOut(), 100);
}

TEST_F(TimedDoorTest, ThrowsExceptionWhenDoorIsOpen) {
    timedDoor->unlock();
    ASSERT_THROW(timedDoor->adapter->Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, DoesNotThrowExceptionWhenDoorIsClosed) {
    timedDoor->lock();
    ASSERT_NO_THROW(timedDoor->adapter->Timeout());
}

TEST(DoorTimerAdapterTest, TimeoutCallsThrowStateOnDoor) {
    class MockTimedDoor : public TimedDoor {
     public:
      MockTimedDoor(int timeout) : TimedDoor(timeout) {}
      MOCK_METHOD(void, throwState, (), (override));
    };

    MockTimedDoor mockDoor(10);
    DoorTimerAdapter adapter(mockDoor);
    EXPECT_CALL(mockDoor, throwState())
        .Times(1);
    adapter.Timeout();
}


TEST(TimerTest, tregisterCallsTimeoutWithMockTimer) {
  MockTimerClient mockClient;
  MockTimer mockTimer;

  EXPECT_CALL(mockTimer, tregister(50, _)) 
      .Times(1);
  EXPECT_CALL(mockClient, Timeout())
      .Times(1);
  mockTimer.tregister(50, &mockClient); 
}

TEST(TimedDoorIntegrationTest, OpenDoorThrowsExceptionAfterTimeoutWithMockTimer) {
  TimedDoor door(50);
  MockTimer mockTimer;
  door.unlock();
  EXPECT_CALL(mockTimer, tregister(door.getTimeOut(), door.adapter))
      .Times(1)
      .WillOnce(Invoke([&door]() { 
          door.adapter->Timeout(); 
      }));
  mockTimer.tregister(door.getTimeOut(), door.adapter); 
  ASSERT_THROW(door.throwState(), std::runtime_error);
  delete door.adapter;
}

TEST(TimedDoorIntegrationTest, ClosedDoorDoesNotThrowExceptionAfterTimeoutWithMockTimer) {
  TimedDoor door(50);
  MockTimer mockTimer;
  door.lock();
    EXPECT_CALL(mockTimer, tregister(door.getTimeOut(), door.adapter))
        .Times(1)
        .WillOnce(Invoke([&door]() { 
            door.adapter->Timeout(); 
        }));
  mockTimer.tregister(door.getTimeOut(), door.adapter);
  ASSERT_NO_THROW(door.throwState());
  delete door.adapter;
}
