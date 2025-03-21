// Copyright 2025 Artem Tyshkevich

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"

using ::testing::_;
using ::testing::AtLeast;

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
    Timer* timer;
    MockTimerClient* mockClient;

    void SetUp() override {
        door = new TimedDoor(5);
        timer = new Timer();
        mockClient = new MockTimerClient();
    }

    void TearDown() override {
        delete door;
        delete timer;
        delete mockClient;
    }
};

TEST_F(TimedDoorTest, ConstructorSetsTimeout) {
    EXPECT_EQ(door->getTimeOut(), 5);
}

TEST_F(TimedDoorTest, InitialStateIsClosed) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
    door->unlock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, ThrowStateWhenClosedNoException) {
    EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, ThrowStateWhenOpenThrows) {
    door->unlock();
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, AdapterTimeoutWhenClosedNoThrow) {
    DoorTimerAdapter adapter(*door);
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST_F(TimedDoorTest, AdapterTimeoutWhenOpenThrows) {
    DoorTimerAdapter adapter(*door);
    door->unlock();
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, TimerRegistrationCallsTimeout) {
    EXPECT_CALL(*mockClient, Timeout()).Times(AtLeast(1));
    timer->tregister(1, mockClient);
}

TEST_F(TimedDoorTest, FullSystemOpenDoorThrowsAfterTimeout) {
    door->unlock();
    DoorTimerAdapter adapter(*door);
    EXPECT_THROW({
        timer->tregister(door->getTimeOut(), &adapter);
    }, std::runtime_error);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
