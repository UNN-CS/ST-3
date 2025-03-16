// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::Throw;
using ::testing::NiceMock;

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
class MockTimer : public Timer {
public:
    MOCK_METHOD(void, tregister, (int, TimerClient*), (override));
    MOCK_METHOD(void, sleep, (int), (override));
};
class TimedDoorTest : public ::testing::Test {
protected:
    void SetUp() override {
        timedDoor = new TimedDoor(5);
        mockTimer = new NiceMock<MockTimer>();
    }

    void TearDown() override {
        delete timedDoor;
        delete mockTimer;
    }

    TimedDoor* timedDoor;
    MockTimer* mockTimer;
};

TEST_F(TimedDoorTest, InitialStateIsLocked) {
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockChangesState) {
    timedDoor->unlock();
    EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, LockChangesState) {
    timedDoor->unlock();
    timedDoor->lock();
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockRegistersTimer) {
    EXPECT_CALL(*mockTimer, tregister(5, _)).Times(1);
    timedDoor->unlock();
}

TEST_F(TimedDoorTest, TimeoutWhenOpenThrows) {
    timedDoor->unlock();
    ASSERT_THROW(timedDoor->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, TimeoutWhenClosedNoThrow) {
    ASSERT_NO_THROW(timedDoor->throwState());
}



TEST(TimerTest, TimerCallsClientTimeout) {
    MockTimerClient client;
    MockTimer timer;

    EXPECT_CALL(client, Timeout()).Times(1);
    timer.tregister(0, &client);
    timer.sleep(0);
}

TEST(DoorTimerAdapterTest, AdapterThrowsWhenOpen) {
    TimedDoor door(5);
    door.unlock();
    DoorTimerAdapter adapter(door);

    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, AdapterNoThrowWhenClosed) {
    TimedDoor door(5);
    DoorTimerAdapter adapter(door);

    EXPECT_NO_THROW(adapter.Timeout());
}

TEST_F(TimedDoorTest, TimeoutValueCorrect) {
    EXPECT_EQ(timedDoor->getTimeOut(), 5);
}

TEST(IntegrationTest, CloseDoorBeforeTimeout) {
    TimedDoor door(1);
    door.unlock();
    door.lock();

    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST_F(TimedDoorTest, DoubleUnlockResetsTimer) {
    EXPECT_CALL(*mockTimer, tregister(5, _)).Times(2);
    timedDoor->unlock();
    timedDoor->unlock();
}

TEST_F(TimedDoorTest, DestructorReleasesResources) {
    TimedDoor* localDoor = new TimedDoor(5);
    ASSERT_NO_THROW(delete localDoor);
}