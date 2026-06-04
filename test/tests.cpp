// Copyright 2021 GHA Test Team

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <stdexcept>
#include <string>
#include <thread>

#include "TimedDoor.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Exactly;
using ::testing::Return;

// Mock for TimerClient — lets us verify Timeout() is called
class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

// Mock for Door — lets us verify lock/unlock/isDoorOpened calls
class MockDoor : public Door {
 public:
  MOCK_METHOD(void, lock, (), (override));
  MOCK_METHOD(void, unlock, (), (override));
  MOCK_METHOD(bool, isDoorOpened, (), (override));
};

// Test Fixture: creates a fresh TimedDoor(3) for each test
class FridgeDoorTest : public ::testing::Test {
 protected:
  TimedDoor* door;

  void SetUp() override { door = new TimedDoor(3); }

  void TearDown() override { delete door; }
};

// Tests

// Test 01 — Door starts in closed state after construction
TEST_F(FridgeDoorTest, InitialStateIsClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

// Test 02 — Alarm is not armed on a freshly-created door
TEST_F(FridgeDoorTest, AlarmNotArmedInitially) {
  EXPECT_FALSE(door->isAlarmArmed());
}

// Test 03 — unlock() opens the door and arms the alarm
TEST_F(FridgeDoorTest, UnlockOpensAndArmsAlarm) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  EXPECT_TRUE(door->isAlarmArmed());
}

// Test 04 — lock() closes the door and disarms the alarm
TEST_F(FridgeDoorTest, LockClosesAndDisarmsAlarm) {
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
  EXPECT_FALSE(door->isAlarmArmed());
}

// Test 05 — getTimeOut() returns the value passed to the constructor
TEST_F(FridgeDoorTest, TimeoutValueMatchesConstructorArgument) {
  EXPECT_EQ(door->getTimeOut(), 3);

  TimedDoor shortDoor(1);
  EXPECT_EQ(shortDoor.getTimeOut(), 1);

  TimedDoor longDoor(10);
  EXPECT_EQ(longDoor.getTimeOut(), 10);
}

// Test 06 — throwState() throws when door is open
TEST_F(FridgeDoorTest, ThrowStateRaisesExceptionWhenOpen) {
  door->unlock();
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

// Test 07 — throwState() does NOT throw when door is closed
TEST_F(FridgeDoorTest, ThrowStateNoExceptionWhenClosed) {
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

// Test 08 — Exception message contains meaningful text
TEST_F(FridgeDoorTest, ExceptionMessageIsDescriptive) {
  door->unlock();
  try {
    door->throwState();
    FAIL() << "Expected std::runtime_error";
  } catch (const std::runtime_error& ex) {
    EXPECT_NE(std::string(ex.what()).size(), 0u);
  }
}

// Test 09 — Adapter's Timeout() throws when the door is open
TEST_F(FridgeDoorTest, AdapterTimeoutThrowsWithOpenDoor) {
  door->unlock();
  DoorTimerAdapter* adapter = door->getAdapter();
  EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

// Test 10 — Adapter's Timeout() does NOT throw with closed door
TEST_F(FridgeDoorTest, AdapterTimeoutSilentWithClosedDoor) {
  door->lock();
  DoorTimerAdapter* adapter = door->getAdapter();
  EXPECT_NO_THROW(adapter->Timeout());
}

// Test 11 — Repeated open/close cycles preserve consistent state
TEST_F(FridgeDoorTest, RepeatedOpenCloseCyclesRemainConsistent) {
  for (int i = 0; i < 20; ++i) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    EXPECT_TRUE(door->isAlarmArmed());

    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
    EXPECT_FALSE(door->isAlarmArmed());
  }
}

// Test 12 — Timer calls Timeout() exactly once after the specified delay
TEST(TimerBehaviorTest, TimerInvokesTimeoutExactlyOnce) {
  Timer timer;
  MockTimerClient mockClient;

  EXPECT_CALL(mockClient, Timeout()).Times(Exactly(1));

  auto t0 = std::chrono::steady_clock::now();
  timer.tregister(1, &mockClient);
  auto t1 = std::chrono::steady_clock::now();

  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
  // Must have waited at least ~1 second
  EXPECT_GE(elapsed.count(), 900);
}

// Test 13 — Timer with zero delay fires almost immediately
TEST(TimerBehaviorTest, ZeroDelayFiresImmediately) {
  Timer timer;
  MockTimerClient mockClient;

  EXPECT_CALL(mockClient, Timeout()).Times(Exactly(1));

  auto t0 = std::chrono::steady_clock::now();
  timer.tregister(0, &mockClient);
  auto t1 = std::chrono::steady_clock::now();

  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
  EXPECT_LT(elapsed.count(), 200);
}

// Test 14 — Full workflow: open door → timer fires → exception raised
TEST(IntegrationTest, AlarmTriggeredAfterTimeoutWithOpenDoor) {
  TimedDoor fridgeDoor(1);
  Timer alarmTimer;

  fridgeDoor.unlock();

  EXPECT_THROW(alarmTimer.tregister(1, fridgeDoor.getAdapter()),
               std::runtime_error);
}

// Test 15 — Full workflow: door closed before timeout → no exception
TEST(IntegrationTest, NeitherAlarmNorExceptionIfDoorClosedBeforeTimeout) {
  TimedDoor fridgeDoor(3);
  Timer alarmTimer;

  fridgeDoor.unlock();

  std::atomic<bool> exceptionCaught(false);
  std::atomic<bool> timerFinished(false);

  std::thread timerThread([&]() {
    try {
      alarmTimer.tregister(3, fridgeDoor.getAdapter());
    } catch (const std::runtime_error&) {
      exceptionCaught = true;
    }
    timerFinished = true;
  });

  // Close the door well before the 3-second timeout expires
  std::this_thread::sleep_for(std::chrono::seconds(1));
  fridgeDoor.lock();

  timerThread.join();

  EXPECT_FALSE(exceptionCaught);
  EXPECT_TRUE(timerFinished);
  EXPECT_FALSE(fridgeDoor.isDoorOpened());
}

// Test 16 — MockDoor: verify that lock/unlock/isDoorOpened can be called
TEST(MockDoorInterfaceTest, InterfaceMethodsAreCallable) {
  MockDoor mockFridge;

  EXPECT_CALL(mockFridge, unlock()).Times(Exactly(1));
  EXPECT_CALL(mockFridge, isDoorOpened())
      .Times(Exactly(1))
      .WillOnce(Return(true));
  EXPECT_CALL(mockFridge, lock()).Times(Exactly(1));

  mockFridge.unlock();
  bool opened = mockFridge.isDoorOpened();
  EXPECT_TRUE(opened);
  mockFridge.lock();
}

// Test 17 — Exception propagates through Timer when client throws
TEST(TimerBehaviorTest, ExceptionFromClientPropagatesOutOfTimer) {
  class AlwaysThrowingClient : public TimerClient {
   public:
    void Timeout() override {
      throw std::runtime_error("deliberate test exception");
    }
  };

  Timer timer;
  AlwaysThrowingClient throwingClient;

  EXPECT_THROW(timer.tregister(0, &throwingClient), std::runtime_error);
}

// Test 18 — Adapter belongs to the door and reflects current door state
TEST(AdapterOwnershipTest, AdapterReflectsLiveDoorState) {
  TimedDoor door(2);
  DoorTimerAdapter* adapter = door.getAdapter();

  // Door closed — adapter must not throw
  EXPECT_NO_THROW(adapter->Timeout());

  // Open the door — same adapter now must throw
  door.unlock();
  EXPECT_THROW(adapter->Timeout(), std::runtime_error);

  // Close it again — adapter must go quiet
  door.lock();
  EXPECT_NO_THROW(adapter->Timeout());
}
