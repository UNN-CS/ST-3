// Copyright 2021 GHA Test Team
// Copyright 2025 Vasilev Sergey

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

class vasilev_s_timed_door_test : public ::testing::Test {
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
    vasilev_s_timed_door_test() : door(1), timer() {}
};

TEST_F(vasilev_s_timed_door_test, Test_Initial_State) {
    EXPECT_FALSE(door.isDoorOpened());
    EXPECT_EQ(door.getTimeOut(), 1);
}

TEST_F(vasilev_s_timed_door_test, Test_Door_AfterUnlock_IsOpened) {
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

TEST_F(vasilev_s_timed_door_test, Test_Door_StateAfterUnlockAndLock) {
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(vasilev_s_timed_door_test, Test_Door_UnlockOpenedDoor_ThrowsException) {
    door.unlock();
    EXPECT_THROW(door.unlock(), std::logic_error);
}

TEST_F(vasilev_s_timed_door_test, Test_Door_LockClosedDoor_ThrowsException) {
    EXPECT_THROW(door.lock(), std::logic_error);
}

TEST_F(vasilev_s_timed_door_test, Test_Door_ThrowState) {
    EXPECT_THROW(door.throwState(), std::runtime_error);
    door.unlock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

class vasilev_s_adapter_test : public ::testing::Test {
 protected:
    TimedDoor door;
    DoorTimerAdapter adapter;

 public:
    vasilev_s_adapter_test() : door(1), adapter(door) {}
};

TEST_F(vasilev_s_adapter_test, Test_Timeout_WhenDoorOpen) {
    door.unlock();
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(vasilev_s_adapter_test, Test_Timeout_WhenDoorClosed) {
    EXPECT_NO_THROW(adapter.Timeout());
}

class vasilev_s_timer_test : public ::testing::Test {
 protected:
    MockTimerClient client;
    Timer timer;
};

TEST_F(vasilev_s_timer_test, Test_Timer_RegisterClient) {
    EXPECT_CALL(client, Timeout()).Times(1);
    timer.sleep = [](int) {};
    timer.tregister(1, &client);
}

TEST_F(vasilev_s_timer_test, Test_Timer_RegisterWithInvalidParameters) {
    EXPECT_THROW(timer.tregister(-1, nullptr), std::invalid_argument);
}

class TimerForTest : public Timer {
 public:
    void sleep(int) override {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
};

class vasilev_s_integration_test : public ::testing::Test {
 protected:
    TimedDoor door;
    DoorTimerAdapter adapter;
    TimerForTest timer;

 public:
    vasilev_s_integration_test() : door(2), adapter(door) {}
};

TEST_F(vasilev_s_integration_test, Test_DoorTimerIntegration) {
    EXPECT_FALSE(door.isDoorOpened());
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
    EXPECT_THROW(timer.tregister(2, &adapter), std::runtime_error);
    door.lock();
    EXPECT_NO_THROW(timer.tregister(2, &adapter));
}
