// Copyright 2025 GHA Test Team
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <chrono>
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

class TimedDoorTestSuite : public ::testing::Test {
 protected:
    void SetUp() override {
        testDoor = new TimedDoor(5);
    }

    void TearDown() override {
        delete testDoor;
    }

    TimedDoor* testDoor;
};

class DoorTimerAdapterTestSuite : public ::testing::Test {
 protected:
    void SetUp() override {
        testTimedDoor = new TimedDoor(3);
        testAdapter = new DoorTimerAdapter(*testTimedDoor);
    }

    void TearDown() override {
        delete testAdapter;
        delete testTimedDoor;
    }

    TimedDoor* testTimedDoor;
    DoorTimerAdapter* testAdapter;
};

TEST_F(TimedDoorTestSuite, ConstructorSetsCorrectInitialState) {
    EXPECT_FALSE(testDoor->isDoorOpened());
    EXPECT_EQ(testDoor->getTimeOut(), 5);
}

TEST_F(TimedDoorTestSuite, UnlockChangesDoorStateToOpen) {
    testDoor->unlock();
    EXPECT_TRUE(testDoor->isDoorOpened());
}

TEST_F(TimedDoorTestSuite, LockChangesDoorStateToClosed) {
    testDoor->unlock();
    EXPECT_TRUE(testDoor->isDoorOpened());
    
    testDoor->lock();
    EXPECT_FALSE(testDoor->isDoorOpened());
}

TEST_F(TimedDoorTestSuite, GetTimeOutReturnsCorrectValue) {
    ASSERT_EQ(testDoor->getTimeOut(), 5);
}

TEST_F(TimedDoorTestSuite, ThrowStateThrowsRuntimeErrorWhenDoorOpen) {
    testDoor->unlock();
    EXPECT_THROW(testDoor->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTestSuite, ThrowStateThrowsExceptionRegardlessOfDoorState) {
    testDoor->lock();
    EXPECT_THROW(testDoor->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTestSuite, MultipleUnlockLockOperationsWorkCorrectly) {
    for (int iteration = 0; iteration < 5; ++iteration) {
        testDoor->unlock();
        EXPECT_TRUE(testDoor->isDoorOpened());
        testDoor->lock();
        EXPECT_FALSE(testDoor->isDoorOpened());
    }
}

TEST_F(DoorTimerAdapterTestSuite, TimeoutThrowsExceptionWhenDoorOpen) {
    testTimedDoor->unlock();
    EXPECT_THROW(testAdapter->Timeout(), std::runtime_error);
}

TEST_F(DoorTimerAdapterTestSuite, TimeoutDoesNotThrowWhenDoorClosed) {
    testTimedDoor->lock();
    EXPECT_NO_THROW(testAdapter->Timeout());
}

TEST_F(DoorTimerAdapterTestSuite, TimeoutBehaviorAfterStateChange) {
    testTimedDoor->unlock();
    EXPECT_THROW(testAdapter->Timeout(), std::runtime_error);
    
    testTimedDoor->lock();
    EXPECT_NO_THROW(testAdapter->Timeout());
    
    testTimedDoor->unlock();
    EXPECT_THROW(testAdapter->Timeout(), std::runtime_error);
}

TEST(TimerFunctionalityTest, TregisterCallsTimeoutImmediately) {
    MockTimerClient mockClient;
    Timer systemTimer;

    EXPECT_CALL(mockClient, Timeout())
        .Times(1);

    systemTimer.tregister(0, &mockClient);
}

TEST(DifferentTimeoutValuesTest, VariousTimeoutConfigurations) {
    TimedDoor doorConfig1(8);
    TimedDoor doorConfig2(20);
    TimedDoor doorConfig3(50);
    TimedDoor doorConfig4(0);
    TimedDoor doorConfig5(100);

    EXPECT_EQ(doorConfig1.getTimeOut(), 8);
    EXPECT_EQ(doorConfig2.getTimeOut(), 20);
    EXPECT_EQ(doorConfig3.getTimeOut(), 50);
    EXPECT_EQ(doorConfig4.getTimeOut(), 0);
    EXPECT_EQ(doorConfig5.getTimeOut(), 100);
}

TEST(NegativeTimeoutHandlingTest, NegativeTimeoutDoesNotBreakFunctionality) {
    TimedDoor negativeDoor(-3);
    EXPECT_EQ(negativeDoor.getTimeOut(), -3);

    negativeDoor.unlock();
    EXPECT_TRUE(negativeDoor.isDoorOpened());
    
    negativeDoor.lock();
    EXPECT_FALSE(negativeDoor.isDoorOpened());
    
    EXPECT_THROW(negativeDoor.throwState(), std::runtime_error);
}

TEST(QuickIntegrationTest, DoorOpenedTriggersException) {
    TimedDoor smartDoor(1);
    DoorTimerAdapter doorAdapter(smartDoor);
    Timer timerSystem;

    smartDoor.unlock();
    ASSERT_TRUE(smartDoor.isDoorOpened());

    EXPECT_THROW(timerSystem.tregister(1, &doorAdapter), std::runtime_error);
}

TEST(StatePersistenceTest, DoorStatePersistsAcrossOperations) {
    TimedDoor persistentDoor(7);
    
    persistentDoor.unlock();
    EXPECT_TRUE(persistentDoor.isDoorOpened());
    
    persistentDoor.lock();
    EXPECT_FALSE(persistentDoor.isDoorOpened());
    
    persistentDoor.unlock();
    EXPECT_TRUE(persistentDoor.isDoorOpened());
    
    persistentDoor.lock();
    EXPECT_FALSE(persistentDoor.isDoorOpened());
}

TEST(MockVerificationTest, VerifyMockDoorMethodCalls) {
    MockDoor mockDoor;
    
    EXPECT_CALL(mockDoor, unlock())
        .Times(2);
    EXPECT_CALL(mockDoor, lock())
        .Times(1);
    EXPECT_CALL(mockDoor, isDoorOpened())
        .WillOnce(testing::Return(true))
        .WillOnce(testing::Return(false));
    
    mockDoor.unlock();
    mockDoor.unlock();
    mockDoor.lock();
    
    EXPECT_TRUE(mockDoor.isDoorOpened());
    EXPECT_FALSE(mockDoor.isDoorOpened());
}

TEST(AdapterTimeoutMultipleStatesTest, AdapterHandlesVariousDoorStates) {
    TimedDoor stateDoor(6);
    DoorTimerAdapter stateAdapter(stateDoor);
    
    stateDoor.unlock();
    EXPECT_THROW(stateAdapter.Timeout(), std::runtime_error);
    
    stateDoor.lock();
    EXPECT_NO_THROW(stateAdapter.Timeout());
    
    stateDoor.unlock();
    EXPECT_THROW(stateAdapter.Timeout(), std::runtime_error);
    
    stateDoor.lock();
    EXPECT_NO_THROW(stateAdapter.Timeout());
}

TEST(ZeroTimeoutDoorTest, DoorWithZeroTimeout) {
    TimedDoor zeroDoor(0);
    DoorTimerAdapter zeroAdapter(zeroDoor);
    
    zeroDoor.unlock();
    EXPECT_TRUE(zeroDoor.isDoorOpened());
    EXPECT_THROW(zeroAdapter.Timeout(), std::runtime_error);
}

TEST(LargeTimeoutDoorTest, DoorWithLargeTimeout) {
    TimedDoor largeDoor(1000);
    EXPECT_EQ(largeDoor.getTimeOut(), 1000);
    
    largeDoor.unlock();
    EXPECT_TRUE(largeDoor.isDoorOpened());
    largeDoor.lock();
    EXPECT_FALSE(largeDoor.isDoorOpened());
}

TEST(ConstCorrectnessTest, GetTimeOutIsConstMethod) {
    const TimedDoor constDoor(10);
    EXPECT_EQ(constDoor.getTimeOut(), 10);
}

TEST(AdapterReferenceTest, AdapterReferencesCorrectDoor) {
    TimedDoor originalDoor(5);
    DoorTimerAdapter adapter(originalDoor);
    
    originalDoor.unlock();
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
    
    originalDoor.lock();
    EXPECT_NO_THROW(adapter.Timeout());
}