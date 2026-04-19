// Copyright 2021 GHA Test Team

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <thread>

#include "TimedDoor.h"

using ::testing::_;
using ::testing::Exactly;

// Mock classes for testing
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
  TimedDoor* door;

  void SetUp() override { door = new TimedDoor(2); }

  void TearDown() override { delete door; }
};

// 1. Test: Door initially closed
TEST_F(TimedDoorTest, t1) { EXPECT_FALSE(door->isDoorOpened()); }

// 2. Test: Door can be locked after unlock
TEST_F(TimedDoorTest, t2) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());

  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

// 3. Test: Get timeout value
TEST_F(TimedDoorTest, t3) {
  TimedDoor testDoor(10);
  EXPECT_EQ(testDoor.getTimeOut(), 10);
}

// 4. Test: throwState throws exception when door is open
TEST_F(TimedDoorTest, t4) {
  door->unlock();
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

// 5. Test: throwState doesn't throw when door is closed
TEST_F(TimedDoorTest, t5) {
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

// 6. Test: DoorTimerAdapter calls throwState
TEST_F(TimedDoorTest, t6) {
  DoorTimerAdapter adapter(*door);
  door->unlock();
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

// 7. Test: Adapter doesn't throw when door closed
TEST_F(TimedDoorTest, t7) {
  DoorTimerAdapter adapter(*door);
  door->lock();
  EXPECT_NO_THROW(adapter.Timeout());
}

// 8. Test: Multiple lock/unlock operations
TEST_F(TimedDoorTest, t8) {
  for (int i = 0; i < 5; ++i) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
  }
}

// 9. Test: Door state transitions
TEST_F(TimedDoorTest, t9) {
  EXPECT_FALSE(door->isDoorOpened());

  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());

  door->lock();
  EXPECT_FALSE(door->isDoorOpened());

  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());

  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

// 10. Test: Timer registration with delay
TEST(TimerTest, t10) {
  Timer timer;
  MockTimerClient mockClient;

  EXPECT_CALL(mockClient, Timeout()).Times(1);

  auto start = std::chrono::high_resolution_clock::now();
  timer.tregister(2, &mockClient);
  auto end = std::chrono::high_resolution_clock::now();

  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  EXPECT_GE(duration.count(), 1900);
  EXPECT_LE(duration.count(), 2100);
}

// 11. Test: Timer with zero timeout
TEST(TimerTest, t11) {
  Timer timer;
  MockTimerClient mockClient;

  EXPECT_CALL(mockClient, Timeout()).Times(1);

  auto start = std::chrono::high_resolution_clock::now();
  timer.tregister(0, &mockClient);
  auto end = std::chrono::high_resolution_clock::now();

  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  EXPECT_LT(duration.count(), 100);
}

// 12. Test: Complete workflow - exception thrown after timeout
TEST(TimedDoorIntegration, t12) {
  TimedDoor door(2);
  DoorTimerAdapter adapter(door);
  Timer timer;

  door.unlock();

  auto start = std::chrono::high_resolution_clock::now();
  EXPECT_THROW(timer.tregister(2, &adapter), std::runtime_error);
  auto end = std::chrono::high_resolution_clock::now();

  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  EXPECT_GE(duration.count(), 1900);
  EXPECT_LE(duration.count(), 2100);
}

// 13. Test: Complete workflow - door closed before timeout
TEST(TimedDoorIntegration, t13) {
  TimedDoor door(5);
  DoorTimerAdapter adapter(door);
  Timer timer;

  door.unlock();

  std::atomic<bool> exceptionThrown(false);
  std::atomic<bool> timerFinished(false);

  std::thread timerThread([&]() {
    try {
      timer.tregister(5, &adapter);
    } catch (const std::runtime_error&) {
      exceptionThrown = true;
    }
    timerFinished = true;
  });

  std::this_thread::sleep_for(std::chrono::seconds(2));
  door.lock();

  timerThread.join();

  EXPECT_FALSE(exceptionThrown);
  EXPECT_TRUE(timerFinished);
  EXPECT_FALSE(door.isDoorOpened());
}

// 14. Test: Different timeout values
TEST(TimedDoorParamTest, t14) {
  TimedDoor shortDoor(1);
  TimedDoor longDoor(3);
  DoorTimerAdapter shortAdapter(shortDoor);
  DoorTimerAdapter longAdapter(longDoor);
  Timer timer;

  shortDoor.unlock();
  auto startShort = std::chrono::high_resolution_clock::now();
  EXPECT_THROW(timer.tregister(1, &shortAdapter), std::runtime_error);
  auto endShort = std::chrono::high_resolution_clock::now();
  auto durationShort = std::chrono::duration_cast<std::chrono::milliseconds>(
      endShort - startShort);
  EXPECT_GE(durationShort.count(), 900);
  EXPECT_LE(durationShort.count(), 1100);

  longDoor.unlock();
  auto startLong = std::chrono::high_resolution_clock::now();
  EXPECT_THROW(timer.tregister(3, &longAdapter), std::runtime_error);
  auto endLong = std::chrono::high_resolution_clock::now();
  auto durationLong = std::chrono::duration_cast<std::chrono::milliseconds>(
      endLong - startLong);
  EXPECT_GE(durationLong.count(), 2900);
  EXPECT_LE(durationLong.count(), 3100);
}

// 15. Test: Mock door interface verification
TEST(MockDoorTest, t15) {
  MockDoor mockDoor;

  EXPECT_CALL(mockDoor, isDoorOpened()).Times(1);
  EXPECT_CALL(mockDoor, lock()).Times(1);
  EXPECT_CALL(mockDoor, unlock()).Times(1);

  mockDoor.isDoorOpened();
  mockDoor.lock();
  mockDoor.unlock();
}

// 16. Test: Timer throws exception from client
TEST(TimerTest, t16) {
  Timer timer;

  class ThrowingClient : public TimerClient {
   public:
    void Timeout() override { throw std::runtime_error("Test exception"); }
  };

  ThrowingClient client;

  EXPECT_THROW(timer.tregister(1, &client), std::runtime_error);
}
