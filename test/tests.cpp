// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"
#include <cstdint>
#include <chrono>
#include <thread>
// Copyright 2021 GHA Test Team

class TimedDoorFixture : public ::testing::Test {
protected:
    TimedDoor* door;
    void SetUp() override {
        door = new TimedDoor(1);
    }
    void TearDown() override {
        // ���� ����� �������� ��������, �������� ��������� ���������� �������
        try {
            door->joinTimerThread();
        }
        catch (...) {
            // ���������� ���������� � TearDown
        }
        delete door;
    }
};

// ���� 1: ��������, ��� ����� ������ lock() ����� �������.
TEST_F(TimedDoorFixture, DoorIsClosedAfterLock) {
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

// ���� 2: ��������, ��� ����� unlock() ����� ������� (�� ������������ �������).
TEST_F(TimedDoorFixture, DoorIsOpenedAfterUnlock) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    EXPECT_THROW(door->joinTimerThread(), std::runtime_error);
}

// ���� 3: �������� ������� ���������� ��� �������� ����� ����� ��������.
TEST_F(TimedDoorFixture, ExceptionThrownWhenDoorRemainsOpen) {
    door->unlock();
    EXPECT_THROW(door->joinTimerThread(), std::runtime_error);
}

// ���� 4: �������� ���������� ����������, ���� ����� ����������� �� ��������� ��������.
TEST_F(TimedDoorFixture, NoExceptionWhenDoorClosedBeforeTimeout) {
    TimedDoor doorLocal(2);
    doorLocal.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    doorLocal.lock();
    EXPECT_NO_THROW(doorLocal.joinTimerThread());
}

// ���� 5: �������� ������������� �������� ��������.
TEST_F(TimedDoorFixture, GetTimeOutReturnsCorrectValue) {
    int timeout = 3;
    TimedDoor doorLocal(timeout);
    EXPECT_EQ(doorLocal.getTimeOut(), timeout);
}

// ���� 6: ������������� mock ��� �������� ������ ������ Timeout ��������.
// �������� ��������� �����-��������� TimerClient.
class MockTimerClient : public TimerClient {
public:
    MOCK_METHOD(void, Timeout, (), (override));
};

TEST(TimedDoorTests, AdapterTimeoutIsCalled) {
    // ������� mock-������
    MockTimerClient mockClient;
    // �������, ��� ����� Timeout ����� ������ ����� 1 ���.
    EXPECT_CALL(mockClient, Timeout()).Times(1);

    Timer timer;
    timer.tregister(0, &mockClient);  // ������� 0 ��� ����������� ������
}

// ���� 7: �������� ������ lock() � ����� ���������� ��������.
TEST_F(TimedDoorFixture, LockClosesDoor) {
    door->unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

// ���� 8: �������� ������ unlock() � ����� ���������� ��������.
TEST_F(TimedDoorFixture, UnlockOpensDoor) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    EXPECT_THROW(door->joinTimerThread(), std::runtime_error);
}

// ���� 9: ��������, ��� joinTimerThread() ��������� ��������� ����� �������.
TEST_F(TimedDoorFixture, JoinTimerThreadWorksCorrectly) {
    door->unlock();
    EXPECT_THROW(door->joinTimerThread(), std::runtime_error);
}

// ���� 10: ��������, ��� ��������� ����� joinTimerThread() (���� ����� ��� ��������) �� �������� � ������.
TEST_F(TimedDoorFixture, MultipleJoinTimerThreadCallsDoNotFail) {
    door->unlock();
    try {
        door->joinTimerThread();
    }
    catch (...) {
        // ���������� ���������� ������� ������
    }
    // ��������� ����� �� ������ ��������� � ����.
    EXPECT_NO_THROW(door->joinTimerThread());
}