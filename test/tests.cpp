// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <chrono>
#include <stdexcept>
#include "TimedDoor.h"

using ::testing::Return;

// ========== МОК-КЛАССЫ ==========

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

// ========== ВСПОМОГАТЕЛЬНЫЕ КЛАССЫ ==========

class TimeoutTrigger {
 public:
    void fire(TimerClient* client) { client->Timeout(); }
};

class DoorController {
 public:
    void open(Door* d)   { d->unlock(); }
    void close(Door* d)  { d->lock(); }
    bool isOpen(Door* d) { return d->isDoorOpened(); }
};

// ========== ТЕСТОВЫЙ ФИКСТУР ==========

class TimedDoorTest : public ::testing::Test {
 protected:
    TimedDoor* door;
    DoorTimerAdapter* adapter;

    void SetUp() override {
        door    = new TimedDoor(1);
        adapter = new DoorTimerAdapter(*door);
    }

    void TearDown() override {
        delete adapter;
        delete door;
    }
};

// ========== ТЕСТЫ TimedDoor ==========

TEST_F(TimedDoorTest, DoorIsClosedAfterCreation) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorRemainsClosedAfterLock) {
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST(TimedDoorConstructorTest, TimeoutValueStoredCorrectly) {
    TimedDoor d(7);
    EXPECT_EQ(d.getTimeOut(), 7);
}

TEST_F(TimedDoorTest, ThrowStateDoesNotThrowWhenDoorClosed) {
    door->lock();
    EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, ThrowStateThrowsWhenDoorIsOpen) {
    // unlock открывает дверь (isOpened=true), таймер срабатывает — бросает исключение
    EXPECT_THROW(door->unlock(), std::runtime_error);
    // дверь всё ещё открыта, throwState тоже должен бросить
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, DoorIsOpenAfterUnlockThrows) {
    EXPECT_THROW(door->unlock(), std::runtime_error);
    EXPECT_TRUE(door->isDoorOpened());
}

TEST(TimedDoorZeroTimeoutTest, UnlockThrowsImmediatelyWithZeroTimeout) {
    TimedDoor d(0);
    EXPECT_THROW(d.unlock(), std::runtime_error);
}

// ========== ТЕСТЫ АДАПТЕРА ==========

TEST_F(TimedDoorTest, AdapterTimeoutOnClosedDoorDoesNotThrow) {
    door->lock();
    EXPECT_NO_THROW(adapter->Timeout());
}

TEST_F(TimedDoorTest, AdapterTimeoutOnOpenDoorThrows) {
    EXPECT_THROW(door->unlock(), std::runtime_error);
    EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

// ========== ТЕСТЫ С МОКАМИ (TimerClient) ==========

TEST(TimerMockTest, TregisterCallsTimeoutExactlyOnce) {
    Timer timer;
    MockTimerClient mockClient;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    timer.tregister(0, &mockClient);
}

TEST(TimeoutTriggerTest, FireInvokesTimeoutOnMock) {
    MockTimerClient mockClient;
    TimeoutTrigger trigger;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    trigger.fire(&mockClient);
}

// ========== ТЕСТЫ С МОКАМИ (Door) ==========

TEST(DoorControllerTest, OpenCallsUnlockOnMockDoor) {
    MockDoor mockDoor;
    DoorController ctrl;
    EXPECT_CALL(mockDoor, unlock()).Times(1);
    ctrl.open(&mockDoor);
}

TEST(DoorControllerTest, CloseCallsLockOnMockDoor) {
    MockDoor mockDoor;
    DoorController ctrl;
    EXPECT_CALL(mockDoor, lock()).Times(1);
    ctrl.close(&mockDoor);
}

TEST(DoorControllerTest, IsOpenReturnsMockedValue) {
    MockDoor mockDoor;
    DoorController ctrl;
    EXPECT_CALL(mockDoor, isDoorOpened()).Times(1).WillOnce(Return(false));
    EXPECT_FALSE(ctrl.isOpen(&mockDoor));
}

// ========== ИНТЕГРАЦИОННЫЙ ТЕСТ ==========

TEST(IntegrationTest, ClosingDoorBeforeTimeoutPreventsException) {
    TimedDoor testDoor(1);

    std::thread closer([&testDoor]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        testDoor.lock();
    });

    EXPECT_NO_THROW(testDoor.unlock());
    closer.join();
    EXPECT_FALSE(testDoor.isDoorOpened());
}

// ВСЕГО ТЕСТОВ: 15
