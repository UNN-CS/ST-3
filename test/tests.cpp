#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"
#include <stdexcept>

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
        door = new TimedDoor(100);
        adapter = new DoorTimerAdapter(*door);
    }
    
    void TearDown() override {
        delete door;
        delete adapter;
    }
    
    TimedDoor* door;
    DoorTimerAdapter* adapter;
};

TEST_F(TimedDoorTest, DoorCreationTest) {
    EXPECT_EQ(door->getTimeOut(), 100);
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorLockUnlockTest) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, AdapterWithClosedDoorNoException) {
    door->lock();
    EXPECT_NO_THROW(adapter->Timeout());
}

TEST_F(TimedDoorTest, AdapterWithOpenedDoorThrowsException) {
    door->unlock();
    EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, ThrowStateThrowsException) {
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, DoorStateChangesTest) {
    door->unlock();
    EXPECT_THROW(adapter->Timeout(), std::runtime_error);
    door->lock();
    EXPECT_NO_THROW(adapter->Timeout());
}

TEST_F(TimedDoorTest, MultipleLockUnlockTest) {
    for (int i = 0; i < 5; i++) {
        door->unlock();
        EXPECT_TRUE(door->isDoorOpened());
        door->lock();
        EXPECT_FALSE(door->isDoorOpened());
    }
}

TEST(TimedDoorDifferentTimeoutsTest, DifferentTimeoutValues) {
    TimedDoor door1(88);
    TimedDoor door2(500);
    TimedDoor door3(999);
    
    EXPECT_EQ(door1.getTimeOut(), 88);
    EXPECT_EQ(door2.getTimeOut(), 500);
    EXPECT_EQ(door3.getTimeOut(), 999);
}

TEST_F(TimedDoorTest, MultipleTimeoutCallsTest) {
    door->lock();
    for (int i = 0; i < 3; i++) {
        EXPECT_NO_THROW(adapter->Timeout());
    }
    
    door->unlock();
    for (int i = 0; i < 3; i++) {
        EXPECT_THROW(adapter->Timeout(), std::runtime_error);
    }
}

TEST(DoorAdapterTest, AdapterWithDifferentDoors) {
    TimedDoor door1(100);
    TimedDoor door2(200);
    
    DoorTimerAdapter adapter1(door1);
    DoorTimerAdapter adapter2(door2);
    
    door1.unlock();
    door2.lock();
    
    EXPECT_THROW(adapter1.Timeout(), std::runtime_error);
    EXPECT_NO_THROW(adapter2.Timeout());
}

TEST(MockTimerClientTest, TimerClientTimeoutCall) {
    MockTimerClient mockClient;
    
    EXPECT_CALL(mockClient, Timeout())
        .Times(1);
    
    mockClient.Timeout();
}

TEST(MockDoorTest, DoorOperationsCall) {
    MockDoor mockDoor;
    
    EXPECT_CALL(mockDoor, unlock()).Times(1);
    EXPECT_CALL(mockDoor, lock()).Times(1);
    EXPECT_CALL(mockDoor, isDoorOpened())
        .WillOnce(testing::Return(true));
    
    mockDoor.unlock();
    mockDoor.lock();
    EXPECT_TRUE(mockDoor.isDoorOpened());
}

TEST_F(TimedDoorTest, ReopenDoorAfterTimeoutTest) {
    door->unlock();
    EXPECT_THROW(adapter->Timeout(), std::runtime_error);
    
    door->lock();
    door->unlock();
    EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, NoExceptionAfterDoorClosedTest) {
    door->unlock();
    EXPECT_THROW(adapter->Timeout(), std::runtime_error);
    
    door->lock();
    EXPECT_NO_THROW(adapter->Timeout());
}