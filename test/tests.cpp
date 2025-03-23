// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <thread>
#include <chrono>
#include <stdexcept>
#include "TimedDoor.h"

class TimedDoorTest : public ::testing::Test {
 protected:
        TimedDoor* door;
        DoorTimerAdapter* adapter;

        void SetUp() override {
            door = new TimedDoor(2);
            adapter = new DoorTimerAdapter(*door);
        }

        void TearDown() override {
            delete adapter;
            delete door;
        }
};

    TEST_F(TimedDoorTest, InitiallyClosed) {
        EXPECT_FALSE(door->isDoorOpened());
    }

    TEST_F(TimedDoorTest, LockDoor) {
        door->unlock();
        door->lock();
        EXPECT_FALSE(door->isDoorOpened());
    }

    TEST_F(TimedDoorTest, UnlockDoor) {
        door->unlock();
        EXPECT_TRUE(door->isDoorOpened());
    }

    TEST_F(TimedDoorTest, TimeoutAfterUnlock) {
        door->unlock();
        std::this_thread::sleep_for(std::chrono::seconds(3));
        EXPECT_ANY_THROW(door->throwState());
    }

    TEST_F(TimedDoorTest, ExceptionThrownWhenDoorOpen) {
        door->unlock();
        EXPECT_ANY_THROW(door->throwState());
    }

    TEST_F(TimedDoorTest, NoExceptionWhenDoorClosed) {
        EXPECT_NO_THROW(door->throwState());
    }

    TEST_F(TimedDoorTest, DoorStateAfterUnlockAndLock) {
        door->unlock();
        EXPECT_TRUE(door->isDoorOpened());
        door->lock();
        EXPECT_FALSE(door->isDoorOpened());
    }

    TEST_F(TimedDoorTest, TimerDoesNotThrowIfDoorIsClosed) {
        EXPECT_NO_THROW(adapter->Timeout());
    }

    TEST_F(TimedDoorTest, ExceptionAfterTimeoutWhenDoorOpen) {
        door->unlock();
        adapter->Timeout();
        EXPECT_ANY_THROW(door->throwState());
    }

    TEST_F(TimedDoorTest, TimerFiresAfterTimeout) {
        door->unlock();
        std::this_thread::sleep_for(std::chrono::seconds(3));
        EXPECT_ANY_THROW(door->throwState());
    }
