// Copyright 2021 GHA Test Team

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <exception>
#include <stdexcept>
#include <thread>

#include "TimedDoor.h"

using ::testing::AtLeast;
using ::testing::Exactly;
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

class DoorUser {
 public:
  void OpenAndCheck(Door& door) {
    door.unlock();
    door.isDoorOpened();
    door.lock();
  }

  bool CheckOnly(Door& door) {
    return door.isDoorOpened();
  }

  void DoubleCheck(Door& door) {
    door.isDoorOpened();
    door.isDoorOpened();
  }
};

class TimerUser {
 public:
  void UseClient(TimerClient& client) {
    client.Timeout();
  }

  void UseClientTwice(TimerClient& client) {
    client.Timeout();
    client.Timeout();
  }
};

class TimedDoorTest : public testing::Test {
 protected:
  TimedDoor* door;

  void SetUp() override {
    door = new TimedDoor(50);
  }

  void TearDown() override {
    delete door;
  }
};

TEST_F(TimedDoorTest, DoorIsInitiallyClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockKeepsDoorClosed) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, MultipleLocksKeepDoorClosed) {
  door->lock();
  door->lock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeoutReturnsConstructorValue) {
  EXPECT_EQ(50, door->getTimeOut());
}

TEST_F(TimedDoorTest, ThrowStateDoesNotThrowWhenDoorClosed) {
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, ThrowStateThrowsWhenDoorOpened) {
  door->lock();
  std::thread open_thread([this]() {
    try {
      door->unlock();
    } catch (...) {
    }
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  EXPECT_TRUE(door->isDoorOpened());
  EXPECT_THROW(door->throwState(), std::runtime_error);
  open_thread.join();
}

TEST_F(TimedDoorTest, UnlockThrowsIfDoorRemainsOpened) {
  EXPECT_THROW(door->unlock(), std::runtime_error);
}

TEST_F(TimedDoorTest, UnlockDoesNotThrowIfDoorIsClosedBeforeTimeout) {
  std::exception_ptr thread_exception = nullptr;

  std::thread open_thread([this, &thread_exception]() {
    try {
      door->unlock();
    } catch (...) {
      thread_exception = std::current_exception();
    }
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  door->lock();
  open_thread.join();

  EXPECT_EQ(nullptr, thread_exception);
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorCanBeOpenedAndThenClosedBeforeTimeout) {
  std::thread open_thread([this]() {
    try {
      door->unlock();
    } catch (...) {
    }
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  EXPECT_TRUE(door->isDoorOpened());
  door->lock();
  open_thread.join();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorIsOpenedShortlyAfterUnlockStarts) {
  std::thread open_thread([this]() {
    try {
      door->unlock();
    } catch (...) {
    }
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  EXPECT_TRUE(door->isDoorOpened());
  open_thread.join();
}

TEST_F(TimedDoorTest, LockAfterFailedUnlockClosesDoor) {
  try {
    door->unlock();
  } catch (...) {
  }
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, ThrowStateStillDoesNotThrowAfterRepeatedLocks) {
  door->lock();
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, TimeoutValueRemainsUnchangedAfterLock) {
  door->lock();
  EXPECT_EQ(50, door->getTimeOut());
}

TEST_F(TimedDoorTest, TimeoutValueRemainsUnchangedAfterUnlockAttempt) {
  try {
    door->unlock();
  } catch (...) {
  }
  EXPECT_EQ(50, door->getTimeOut());
}

TEST_F(TimedDoorTest, NewDoorWithZeroTimeoutStoresZero) {
  TimedDoor zero_door(0);
  EXPECT_EQ(0, zero_door.getTimeOut());
}

TEST_F(TimedDoorTest, ClosedDoorStateCanBeCheckedManyTimes) {
  EXPECT_FALSE(door->isDoorOpened());
  EXPECT_FALSE(door->isDoorOpened());
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorRemainsClosedIfOnlyLockWasCalled) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, ManualThrowStateOnClosedDoorIsSafe) {
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, UnlockOpensDoorBeforeException) {
  std::thread open_thread([this]() {
    try {
      door->unlock();
    } catch (...) {
    }
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  EXPECT_TRUE(door->isDoorOpened());
  open_thread.join();
}

TEST_F(TimedDoorTest, DoorCanBeClosedAfterSuccessfulNoThrowScenario) {
  std::exception_ptr thread_exception = nullptr;

  std::thread open_thread([this, &thread_exception]() {
    try {
      door->unlock();
    } catch (...) {
      thread_exception = std::current_exception();
    }
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  door->lock();
  open_thread.join();

  EXPECT_EQ(nullptr, thread_exception);
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST(MockDoorTest, UserCallsDoorMethods) {
  MockDoor mock_door;
  DoorUser user;

  EXPECT_CALL(mock_door, unlock()).Times(Exactly(1));
  EXPECT_CALL(mock_door, isDoorOpened()).Times(Exactly(1));
  EXPECT_CALL(mock_door, lock()).Times(Exactly(1));

  user.OpenAndCheck(mock_door);
}

TEST(MockDoorTest, CheckOnlyCallsDoorStateOnce) {
  MockDoor mock_door;
  DoorUser user;

  EXPECT_CALL(mock_door, isDoorOpened()).Times(Exactly(1))
      .WillOnce(Return(true));

  EXPECT_TRUE(user.CheckOnly(mock_door));
}

TEST(MockDoorTest, DoubleCheckCallsDoorStateTwice) {
  MockDoor mock_door;
  DoorUser user;

  EXPECT_CALL(mock_door, isDoorOpened()).Times(Exactly(2))
      .WillRepeatedly(Return(false));

  user.DoubleCheck(mock_door);
}

TEST(MockDoorTest, UserDoesNotCallLockInCheckOnly) {
  MockDoor mock_door;
  DoorUser user;

  EXPECT_CALL(mock_door, lock()).Times(Exactly(0));
  EXPECT_CALL(mock_door, unlock()).Times(Exactly(0));
  EXPECT_CALL(mock_door, isDoorOpened()).Times(Exactly(1))
      .WillOnce(Return(false));

  EXPECT_FALSE(user.CheckOnly(mock_door));
}

TEST(MockTimerClientTest, UserCallsTimeoutMethod) {
  MockTimerClient mock_client;
  TimerUser user;

  EXPECT_CALL(mock_client, Timeout()).Times(Exactly(1));

  user.UseClient(mock_client);
}

TEST(MockTimerClientTest, UserCanCallTimeoutTwice) {
  MockTimerClient mock_client;
  TimerUser user;

  EXPECT_CALL(mock_client, Timeout()).Times(Exactly(2));

  user.UseClientTwice(mock_client);
}

TEST(TimerTest, TimerCallsClientTimeout) {
  Timer timer;
  MockTimerClient mock_client;

  EXPECT_CALL(mock_client, Timeout()).Times(Exactly(1));

  timer.tregister(1, &mock_client);
}

TEST(TimerTest, TimerCanCallTimeoutMoreThanOnce) {
  Timer timer;
  MockTimerClient mock_client;

  EXPECT_CALL(mock_client, Timeout()).Times(AtLeast(2));

  timer.tregister(1, &mock_client);
  timer.tregister(1, &mock_client);
}

TEST(TimerTest, TimerWithZeroTimeoutStillCallsClient) {
  Timer timer;
  MockTimerClient mock_client;

  EXPECT_CALL(mock_client, Timeout()).Times(Exactly(1));

  timer.tregister(0, &mock_client);
}

TEST(AdapterStyleTest, AdapterCallsThrowStateForOpenedDoor) {
  TimedDoor local_door(1);
  DoorTimerAdapter adapter(local_door);

  std::thread open_thread([&local_door]() {
    try {
      local_door.unlock();
    } catch (...) {
    }
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
  open_thread.join();
}
