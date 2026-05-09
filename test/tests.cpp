#include "TimedDoor.h"
#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <thread>

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
    testDoor = new TimedDoor(1);
    testAdapter = new DoorTimerAdapter(*testDoor);
    testDoor->lock();
  }

  void TearDown() override {
    delete testAdapter;
    delete testDoor;
  }
  TimedDoor *testDoor;
  DoorTimerAdapter *testAdapter;
};

TEST_F(TimedDoorTest, checkInitialStateIsClosed) {
  ASSERT_FALSE(testDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, verifyOpenCloseSequence) {
  testDoor->unlock();
  ASSERT_TRUE(testDoor->isDoorOpened());
  testDoor->lock();
  ASSERT_FALSE(testDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, verifyTimeoutValueStored) {
  ASSERT_EQ(testDoor->getTimeOut(), 1);
}

TEST_F(TimedDoorTest, verifyNoExceptionWhenDoorClosed) {
  testDoor->lock();
  ASSERT_NO_THROW(testDoor->throwState());
}

TEST_F(TimedDoorTest, verifyExceptionAfterTimeout) {
  testDoor->unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(1100));
  ASSERT_ANY_THROW(testDoor->lock());
}

TEST_F(TimedDoorTest, verifyTimerTriggersCallback) {
  MockTimerClient mockClient;
  Timer timer;
  EXPECT_CALL(mockClient, Timeout()).Times(1);
  timer.tregister(1, &mockClient);
  std::this_thread::sleep_for(std::chrono::seconds(2));
}

TEST_F(TimedDoorTest, verifyDoorOpensAfterUnlock) {
  testDoor->unlock();
  ASSERT_TRUE(testDoor->isDoorOpened());
  testDoor->lock();
}

TEST_F(TimedDoorTest, verifyDoorClosesAfterLock) {
  testDoor->unlock();
  testDoor->lock();
  ASSERT_FALSE(testDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, verifyNoThrowAfterShortDelay) {
  testDoor->unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  ASSERT_NO_THROW(testDoor->throwState());
}

TEST_F(TimedDoorTest, verifyThrowAfterLongDelay) {
  testDoor->unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  ASSERT_ANY_THROW(testDoor->throwState());
}

TEST_F(TimedDoorTest, verifyLockThrowsAfterTimeout) {
  testDoor->unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(1100));
  ASSERT_THROW(testDoor->lock(), std::runtime_error);
}

TEST_F(TimedDoorTest, mockDoorReturnsFalseState) {
  MockDoor mockDoor;
  EXPECT_CALL(mockDoor, isDoorOpened()).WillOnce(::testing::Return(false));
  ASSERT_FALSE(mockDoor.isDoorOpened());
}

TEST_F(TimedDoorTest, mockDoorLockCalledOnce) {
  MockDoor mockDoor;
  EXPECT_CALL(mockDoor, lock()).Times(1);
  mockDoor.lock();
}

TEST_F(TimedDoorTest, mockDoorUnlockCalledOnce) {
  MockDoor mockDoor;
  EXPECT_CALL(mockDoor, unlock()).Times(1);
  mockDoor.unlock();
}

TEST_F(TimedDoorTest, timerCallsTimeoutExactlyOnce) {
  MockTimerClient mockClient;
  Timer timer;
  EXPECT_CALL(mockClient, Timeout()).Times(1);
  timer.tregister(1, &mockClient);
  std::this_thread::sleep_for(std::chrono::seconds(2));
}

TEST_F(TimedDoorTest, doorStateAfterUnlockLockUnlock) {
  ASSERT_FALSE(testDoor->isDoorOpened());
  testDoor->unlock();
  ASSERT_TRUE(testDoor->isDoorOpened());
  testDoor->lock();
  ASSERT_FALSE(testDoor->isDoorOpened());
  testDoor->unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  ASSERT_NO_THROW(testDoor->throwState());
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  ASSERT_THROW(testDoor->lock(), std::runtime_error);
}