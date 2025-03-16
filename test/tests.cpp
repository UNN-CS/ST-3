// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::Throw;
using ::testing::NiceMock;
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
class MockTimer : public Timer {
 public:
    MOCK_METHOD(void, tregister, (int, TimerClient*), ());
    MOCK_METHOD(void, sleep, (int), ());
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

TEST_F(TimedDoorTest, TimeoutWhenOpenThrows) {
    timedDoor->unlock();
    ASSERT_THROW(timedDoor->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, TimeoutWhenClosedNoThrow) {
    ASSERT_NO_THROW(timedDoor->throwState());
}

TEST_F(TimedDoorTest, TimeoutValueCorrect) {
    EXPECT_EQ(timedDoor->getTimeOut(), 5);
}

TEST_F(TimedDoorTest, DestructorReleasesResources) {
    TimedDoor* localDoor = new TimedDoor(5);
    ASSERT_NO_THROW(delete localDoor);
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



TEST(IntegrationTest, CloseDoorBeforeTimeout) {
    TimedDoor door(1);
    door.unlock();
    door.lock();

    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
}
