// Copyright 2025 Kapustin Ivan

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <cstdint>
#include "TimedDoor.h"


class MockDoor : public Door {
 public:
  MOCK_METHOD(void, lock, (), (override));
  MOCK_METHOD(void, unlock, (), (override));
  MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
  TimedDoor* door;
	void SetUp() override {
      door = new TimedDoor(1);
	}
	void TearDown() override {
	  delete door;
	}
};

class TimerTest : public ::testing::Test {
protected:
	Timer timer;
};

TEST_F(TimedDoorTest, DoorStartsClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorOpens) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorCloses) {
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeout) {
  EXPECT_EQ(door->getTimeOut(), 1);
}

TEST_F(TimerTest, TimerCallsTimeout) {
  MockTimerClient mockClient;
  EXPECT_CALL(mockClient, Timeout()).Times(1);
  timer.tregister(1, &mockClient);
}

TEST(DoorTimerAdapterTest, ThrowsIfDoorIsOpen) {
  TimedDoor door(1);
  DoorTimerAdapter adapter(door);

  door.unlock();

  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, ThrowsWhenDoorLeftOpen) {
  door->unlock();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, NoThrowIfDoorClosed) {
  door->unlock();
  door->lock();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimerTest, TimerDoesNotCallTimeoutWithoutClient) {
  EXPECT_NO_THROW(timer.tregister(1, nullptr));
}

TEST_F(TimedDoorTest, NoTimeoutIfDoorClosed) {
  door->lock();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  EXPECT_NO_THROW(door->throwState());
}