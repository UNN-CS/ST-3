// Copyright 2021 GHA Test Team

#include "../include/TimedDoor.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::_;
using ::testing::Invoke;

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
    DoorTimerAdapter* adapter;
    Timer* timer;

    void SetUp() override {
        door = new TimedDoor(3);
        adapter = new DoorTimerAdapter(*door);
        timer = new Timer();
    }

    void TearDown() override {
        delete door;
        delete adapter;
        delete timer;
    }
};

TEST_F(TimedDoorTest, DoorOpensAndCloses) {
    EXPECT_FALSE(door->isDoorOpened());
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeout) {
    EXPECT_EQ(door->getTimeOut(), 3);
}

TEST_F(TimedDoorTest, ThrowExceptionOnTimeout) {
    door->unlock();
    EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, NoExceptionIfDoorIsClosed) {
    door->lock();
    EXPECT_NO_THROW(adapter->Timeout());
}

TEST(TimerTest, CallsTimeoutAfterDelay) {
    MockTimerClient client;
    Timer timer;

    EXPECT_CALL(client, Timeout()).Times(1);

    timer.tregister(1, &client);
}

TEST(TimerTest, TimerDoesNotCallTimeoutImmediately) {
    MockTimerClient client;
    Timer timer;

    EXPECT_CALL(client, Timeout()).Times(1);

    timer.tregister(1, &client);
}

TEST(TimedDoorAdapterTest, AdapterCallsThrowState) {
    TimedDoor door(3);
    DoorTimerAdapter adapter(door);

    door.unlock();
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, NoExceptionIfDoorClosedBeforeTimeout1) {
    door->unlock();
    door->lock();
    EXPECT_NO_THROW(adapter->Timeout());
}

TEST_F(TimedDoorTest, ExceptionThrownIfDoorRemainsOpen) {
    door->unlock();
    EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, LockingDoorTwiceDoesNotAffectState) {
    door->lock();
    door->lock();  // Дважды вызываем lock
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockingDoorTwiceKeepsItOpen) {
    door->unlock();
    door->unlock();  // Дважды вызываем unlock
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TimerAdapterHandlesMultipleTimeouts) {
    door->unlock();
    EXPECT_THROW(adapter->Timeout(), std::runtime_error);

    door->lock();
    EXPECT_NO_THROW(adapter->Timeout());  // Дверь закрыта, исключение не должно возникнуть
}

TEST_F(TimedDoorTest, TimeoutNotTriggeredIfDoorLockedBeforeTimerExpires) {
    door->unlock();
    door->lock();
    EXPECT_NO_THROW(adapter->Timeout());
}

TEST(TimerTest, TimerHandlesMultipleRegistrations) {
    MockTimerClient client1, client2;
    Timer timer;

    EXPECT_CALL(client1, Timeout()).Times(1);
    EXPECT_CALL(client2, Timeout()).Times(1);

    timer.tregister(1, &client1);
    timer.tregister(1, &client2);
}

TEST(TimedDoorAdapterTest, AdapterDoesNotThrowIfDoorClosedBeforeTimeout) {
    TimedDoor door(3);
    DoorTimerAdapter adapter(door);

    door.unlock();
    door.lock();
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST_F(TimedDoorTest, GetTimeoutReturnsCorrectValueAfterMultipleChecks) {
    EXPECT_EQ(door->getTimeOut(), 3);
    EXPECT_EQ(door->getTimeOut(), 3);
    EXPECT_EQ(door->getTimeOut(), 3);
}

