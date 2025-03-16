// Copyright 2025 Vladlen Korablev

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <chrono>
#include <thread>
#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class korablev_v_timed_door_test : public ::testing::Test {
 protected:
    void SetUp() override {
        timerClient = new MockTimerClient();
    }

    void TearDown() override {
        delete timerClient;
    }

    TimedDoor door;
    Timer timer;
    MockTimerClient* timerClient{};

 public:
    korablev_v_timed_door_test() : door(5), timer() {}
};

TEST_F(korablev_v_timed_door_test, Test_DoorTimerAdapter_Construction) {
    EXPECT_NO_THROW(DoorTimerAdapter adapter(door));
}

TEST_F(korablev_v_timed_door_test, Test_Timer_RegisterWithNullClient) {
    EXPECT_THROW(timer.tregister(5, nullptr), std::invalid_argument);
}

TEST_F(korablev_v_timed_door_test, Test_Door_AfterCreation_IsClosed) {
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(korablev_v_timed_door_test, Test_Door_AfterUnlock_IsOpened) {
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

TEST_F(korablev_v_timed_door_test, Test_Door_LockClosedDoor_ThrowsException) {
    EXPECT_ANY_THROW(door.lock());
}

TEST_F(korablev_v_timed_door_test, Test_Door_UnlockOpenedDoor_ThrowsException) {
    door.unlock();
    EXPECT_ANY_THROW(door.unlock());
}

TEST_F(korablev_v_timed_door_test, Test_Door_LockAfterTimerRegister) {
    timer.tregister(door.getTimeOut(), timerClient);
    EXPECT_ANY_THROW(door.lock());
}

TEST_F(korablev_v_timed_door_test, Test_Door_LockAfterUnlock_DoesNotThrow) {
    door.unlock();
    EXPECT_NO_THROW(door.lock());
}

TEST_F(korablev_v_timed_door_test, Test_Door_StateAfterUnlockAndLock) {
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(korablev_v_timed_door_test, Test_Door_LockBeforeTimeout) {
    door.unlock();
    timer.tregister(door.getTimeOut(), timerClient);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(korablev_v_timed_door_test, Test_Door_ThrowStateAfterTimeout) {
    door.unlock();
    timer.tregister(door.getTimeOut(), timerClient);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    EXPECT_ANY_THROW(door.throwState());
}

TEST_F(korablev_v_timed_door_test, Test_Door_LockBeforeTimeout_T) {
    door.unlock();
    timer.tregister(door.getTimeOut(), timerClient);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    EXPECT_NO_THROW(door.lock());
}
