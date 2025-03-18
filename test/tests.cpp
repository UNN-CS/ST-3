// Copyright 2021 GHA Test Team

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>

#include "TimedDoor.h"

TEST(timed_door_tests, test_num_1) {
  TimedDoor door(5);
  door.unlock();
  door.unlock();
  EXPECT_TRUE(door.isDoorOpened(), false);
}
