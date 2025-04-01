// Copyright 2021 GHA Test Team

#include "TimedDoor.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(TimedDoorTest, InitialStateIsClosed) {
    TimedDoor door(5000);
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, UnlockOpensDoor) {
    TimedDoor door(5000);
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

TEST(TimedDoorTest, LockClosesDoor) {
    TimedDoor door(5000);
    door.unlock();
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, TimeoutIsSetCorrectly) {
    int timeout = 3000;
    TimedDoor door(timeout);
    EXPECT_EQ(door.getTimeOut(), timeout);
}

TEST(TimedDoorTest, TimerThrowsWhenDoorIsOpen) {
    TimedDoor door(5000);
    door.unlock();
    
    RealTimer* timer = dynamic_cast<RealTimer*>(door.getTimer());
    ASSERT_NE(timer, nullptr);
    
    EXPECT_THROW(timer->trigger(), std::runtime_error);
}

TEST(TimedDoorTest, TimerNoThrowWhenDoorIsClosed) {
    TimedDoor door(5000);
    door.unlock();
    door.lock();
    
    RealTimer* timer = dynamic_cast<RealTimer*>(door.getTimer());
    ASSERT_NE(timer, nullptr);
    
    EXPECT_NO_THROW(timer->trigger());
}

TEST(DoorTimerAdapterTest, AdapterThrowsWhenDoorIsOpen) {
    TimedDoor door(5000);
    DoorTimerAdapter adapter(door);
    door.unlock();
    
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(DoorTimerAdapterTest, AdapterNoThrowWhenDoorIsClosed) {
    TimedDoor door(5000);
    DoorTimerAdapter adapter(door);
    door.lock();
    
    EXPECT_NO_THROW(adapter.Timeout());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
  adapter.Timeout();
  adapter.Timeout();
}
