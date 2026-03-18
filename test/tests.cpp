// Copyright 2021 GHA Test Team

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <stdexcept>

#include "TimedDoor.h"

using ::testing::Return;

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

class MockTimedDoor : public TimedDoor {
 public:
  MockTimedDoor() : TimedDoor(0) {}

  MOCK_METHOD(void, throwState, (), (override));
};

class SilentTimedDoor : public TimedDoor {
 public:
  explicit SilentTimedDoor(int timeout) : TimedDoor(timeout) {}

  void throwState() override {}
};

class CountingTimedDoor : public TimedDoor {
 public:
  explicit CountingTimedDoor(int timeout) : TimedDoor(timeout), timeoutCalls(0) {}

  void throwState() override {
    ++timeoutCalls;
  }

  int timeoutCalls;
};

class TimedDoorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    regularDoor = new TimedDoor(0);
    silentDoor = new SilentTimedDoor(0);
    countingDoor = new CountingTimedDoor(0);
  }

  void TearDown() override {
    delete regularDoor;
    delete silentDoor;
    delete countingDoor;
  }

  TimedDoor* regularDoor = nullptr;
  SilentTimedDoor* silentDoor = nullptr;
  CountingTimedDoor* countingDoor = nullptr;
};

TEST_F(TimedDoorTest, ConstructorCreatesClosedDoor) {
  EXPECT_FALSE(regularDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, ConstructorStoresTimeoutValue) {
  EXPECT_EQ(0, regularDoor->getTimeOut());
}

TEST_F(TimedDoorTest, LockKeepsDoorClosed) {
  regularDoor->lock();

  EXPECT_FALSE(regularDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
  silentDoor->unlock();

  EXPECT_TRUE(silentDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockCallsTimeoutHandlerOnce) {
  countingDoor->unlock();

  EXPECT_EQ(1, countingDoor->timeoutCalls);
}

TEST_F(TimedDoorTest, LockClosesPreviouslyOpenedDoor) {
  silentDoor->unlock();
  silentDoor->lock();

  EXPECT_FALSE(silentDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, ThrowStateDoesNotThrowForClosedDoor) {
  EXPECT_NO_THROW(regularDoor->throwState());
}

TEST_F(TimedDoorTest, UnlockThrowsWhenDoorStaysOpenedAfterTimeout) {
  EXPECT_THROW(regularDoor->unlock(), std::runtime_error);
}

TEST(TimerTest, RegisterCallsClientTimeout) {
  Timer timer;
  MockTimerClient client;

  EXPECT_CALL(client, Timeout()).Times(1);

  timer.tregister(0, &client);
}

TEST(TimerTest, RegisterWithNullClientDoesNotThrow) {
  Timer timer;

  EXPECT_NO_THROW(timer.tregister(0, nullptr));
}

TEST(DoorTimerAdapterTest, TimeoutDelegatesToDoorThrowState) {
  MockTimedDoor door;
  DoorTimerAdapter adapter(door);

  EXPECT_CALL(door, throwState()).Times(1);

  adapter.Timeout();
}

TEST(DoorInterfaceTest, MockDoorSupportsInterfaceCalls) {
  MockDoor door;
  Door* doorInterface = &door;

  EXPECT_CALL(door, lock()).Times(1);
  EXPECT_CALL(door, unlock()).Times(1);
  EXPECT_CALL(door, isDoorOpened()).Times(1).WillOnce(Return(true));

  doorInterface->lock();
  doorInterface->unlock();
  EXPECT_TRUE(doorInterface->isDoorOpened());
}
