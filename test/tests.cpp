#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <thread>
#include <memory>

#include "TimedDoor.h"

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
    door = new TimedDoor(2);
  }

  void TearDown() override {
    delete door;
  }

  TimedDoor *door;
};

class DoorTimerAdapterTest : public ::testing::Test {
 protected:
  void SetUp() override {
    door = new TimedDoor(2);
    adapter = std::make_unique<DoorTimerAdapter>(*door);
  }

  void TearDown() override {
    delete door;
  }

  TimedDoor *door;
  std::unique_ptr<DoorTimerAdapter> adapter;
};

TEST_F(TimedDoorTest, InitialStateIsClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeOutReturnsCorrectValue) {
  EXPECT_EQ(door->getTimeOut(), 2);
}

TEST_F(TimedDoorTest, LockAndUnlockChangeState) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, MultipleUnlocksDoNotCreateMultipleTimers) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, ThrowStateThrowsExceptionWhenDoorOpen) {
  door->unlock();
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, ThrowStateDoesNotThrowWhenDoorClosed) {
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(DoorTimerAdapterTest, AdapterTimeoutThrowsWhenDoorOpen) {
  door->unlock();
  EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

TEST_F(DoorTimerAdapterTest, AdapterTimeoutNoThrowWhenDoorClosed) {
  door->lock();
  EXPECT_NO_THROW(adapter->Timeout());
}

TEST(TimerTest, TimerCallsTimeoutOnClient) {
  Timer timer;
  MockTimerClient mockClient;
  
  EXPECT_CALL(mockClient, Timeout())
      .Times(1);
  
  timer.tregister(0, &mockClient);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(TimedDoorIntegrationTest, DoorThrowsExceptionAfterTimeout) {
  TimedDoor door(1);
  
  door.unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(1500));
  EXPECT_TRUE(door.isDoorOpened());
  EXPECT_THROW(door.throwState(), std::runtime_error);
}
TEST(TimedDoorIntegrationTest, ClosingDoorBeforeTimeoutPreventsException) {
  TimedDoor door(2);
  
  door.unlock();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  door.lock();
  
  std::this_thread::sleep_for(std::chrono::seconds(2));
  
  EXPECT_NO_THROW(door.throwState());
}

TEST(MockDoorTest, MockDoorCanVerifyMethodCalls) {
  MockDoor mockDoor;
  
  EXPECT_CALL(mockDoor, unlock())
      .Times(1);
  EXPECT_CALL(mockDoor, lock())
      .Times(1);
  EXPECT_CALL(mockDoor, isDoorOpened())
      .WillOnce(testing::Return(true))
      .WillOnce(testing::Return(false));
  
  mockDoor.unlock();
  EXPECT_TRUE(mockDoor.isDoorOpened());
  mockDoor.lock();
  EXPECT_FALSE(mockDoor.isDoorOpened());
}

TEST_F(TimedDoorTest, ExceptionMessageIsCorrect) {
  door->unlock();
  
  try {
    door->throwState();
    FAIL() << "Expected std::runtime_error";
  } catch (const std::runtime_error& e) {
    EXPECT_STREQ(e.what(), "Door timeout: still open");
  }
}

TEST_F(TimedDoorTest, SequentialOpenCloseOperations) {
  for (int i = 0; i < 5; ++i) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
  }
}