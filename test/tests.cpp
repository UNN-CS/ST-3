// test/tests.cpp
// Copyright 2021 GHA Test Team
using ::testing::AtLeast;

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

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

TEST_F(TimedDoorTest, DoorInitialStateIsClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockSetsDoorToOpen) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  door->lock();
  std::this_thread::sleep_for(std::chrono::milliseconds(600));
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockSetsDoorToClosed) {
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
  std::this_thread::sleep_for(std::chrono::seconds(1));
}

TEST_F(TimedDoorTest, GetTimeOutReturnsCorrectValue) {
  EXPECT_EQ(door->getTimeOut(), 1);
}

TEST_F(TimedDoorTest, ExceptionThrownWhenDoorRemainsOpenAfterTimeout) {
  door->unlock();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, NoExceptionWhenDoorClosedBeforeTimeout) {
  door->unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  door->lock();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  EXPECT_NO_THROW(door->throwState());
}

TEST(TimerAdapterTest, TimeoutCallsThrowStateIfDoorOpen) {
  TimedDoor door(0);
  door.unlock();
  EXPECT_THROW(door.throwState(), std::runtime_error);
}

class TestTimer {
 public:
  void tregister(int timeout, TimerClient* client) {
    std::this_thread::sleep_for(std::chrono::seconds(timeout));
    if (client) {
      client->Timeout();
    }
  }
};

TEST(TimerTest, TimerRegistersAndCallsTimeout) {
  MockTimerClient mockClient;
  TestTimer timer;
  EXPECT_CALL(mockClient, Timeout()).Times(1);
  timer.tregister(1, &mockClient);
}

TEST(TimerTest, TimerWithZeroTimeoutCallsImmediately) {
  MockTimerClient mockClient;
  TestTimer timer;
  EXPECT_CALL(mockClient, Timeout()).Times(1);
  timer.tregister(0, &mockClient);
}

TEST_F(TimedDoorTest, DoorThrowStateThrowsRuntimeError) {
  EXPECT_THROW(door->throwState(), std::runtime_error);
}
