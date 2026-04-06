// Copyright 2021 GHA Test Team
#include "TimedDoor.h"

#include <chrono>
#include <stdexcept>
#include <thread>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::AtLeast;
using ::testing::Exactly;

// ==================== Набор тестов для класса TimedDoor ====================

class TimedDoorTest : public ::testing::Test {
 protected:
  TimedDoor *door;

  void SetUp() override { door = new TimedDoor(2); }
  void TearDown() override { delete door; }
};

// Тест 1: Проверяет, что дверь изначально закрыта (isOpened == false)
TEST_F(TimedDoorTest, DoorInitiallyClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

// Тест 2: Проверяет, что значение таймаута, переданное в конструктор,
// сохраняется
TEST_F(TimedDoorTest, TimeoutValueStored) { EXPECT_EQ(2, door->getTimeOut()); }

// Тест 3: Проверяет, что вызов unlock() переводит дверь в открытое состояние
TEST_F(TimedDoorTest, UnlockOpensDoor) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

// Тест 4: Проверяет, что вызов lock() после открытия закрывает дверь
TEST_F(TimedDoorTest, LockClosesDoor) {
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

// Тест 5: Проверяет, что метод throwState() всегда генерирует исключение
// std::runtime_error
TEST_F(TimedDoorTest, ThrowStateThrowsRuntimeError) {
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

// ==================== Набор тестов для DoorTimerAdapter ====================

class DoorTimerAdapterTest : public ::testing::Test {
 protected:
  TimedDoor *door;
  DoorTimerAdapter *adapter;
  void SetUp() override {
    door = new TimedDoor(1);
    adapter = new DoorTimerAdapter(*door);
  }
  void TearDown() override {
    delete door;
    delete adapter;
  }
};

// Тест 6: Проверяет, что если дверь открыта, вызов Timeout() адаптера
// генерирует исключение
TEST_F(DoorTimerAdapterTest, TimeoutThrowsIfDoorOpen) {
  door->unlock();
  EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

// Тест 7: Проверяет, что если дверь закрыта, вызов Timeout() адаптера НЕ
// генерирует исключение
TEST_F(DoorTimerAdapterTest, TimeoutNoThrowIfDoorClosed) {
  door->lock();
  EXPECT_NO_THROW(adapter->Timeout());
}

// ==================== Мок-класс для тестирования таймера ====================

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

// Тест 8: Проверяет, что таймер действительно вызывает метод Timeout() у
// зарегистрированного клиента
TEST(TimerTest, TimerCallsTimeout) {
  MockTimerClient client;
  EXPECT_CALL(client, Timeout()).Times(Exactly(1));
  Timer timer;
  timer.tregister(0, &client);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

// Тест 9: Проверяет, что после остановки рабочего потока таймер больше не
// вызывает Timeout()
TEST(TimerTest, TimerDoesNotCallAfterStop) {
  MockTimerClient client;
  EXPECT_CALL(client, Timeout()).Times(Exactly(0));
  Timer timer;
  timer.tregister(1, &client);
  timer.stopWorker(); // предполагается, что в Timer добавлен этот метод для
                      // тестов
  std::this_thread::sleep_for(std::chrono::milliseconds(1100));
}

// ==================== Вспомогательный адаптер для интеграционных тестов
// ==================== Перехватывает исключение, выбрасываемое в Timeout(), и
// сохраняет его в exception_ptr

class TestableDoorTimerAdapter : public DoorTimerAdapter {
 private:
  std::exception_ptr *storedException;

 public:
  TestableDoorTimerAdapter(TimedDoor &door, std::exception_ptr *ex)
      : DoorTimerAdapter(door), storedException(ex) {}
  void Timeout() override {
    try {
      DoorTimerAdapter::Timeout();
    } catch (...) {
      *storedException = std::current_exception();
    }
  }
};

// Интеграционный тест: проверяет совместную работу двери, адаптера и таймера
class IntegrationTest : public ::testing::Test {
 protected:
  TimedDoor *door;
  TestableDoorTimerAdapter *testAdapter;
  std::exception_ptr exception;
  void SetUp() override {
    door = new TimedDoor(1);
    testAdapter = new TestableDoorTimerAdapter(*door, &exception);
    door->changeAdapter(testAdapter); // подменяем адаптер в двери
  }
  void TearDown() override { delete door; }
};

// Тест 10: Интеграция – если дверь остаётся открытой дольше таймаута,
// генерируется исключение
TEST_F(IntegrationTest, ThrowsWhenDoorOpenAfterTimeout) {
  door->unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(1050));
  ASSERT_NE(exception, nullptr);
}

// Тест 11: Интеграция – если дверь закрывается до истечения таймаута,
// исключение НЕ генерируется
TEST_F(IntegrationTest, NoThrowWhenDoorClosedBeforeTimeout) {
  door->unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  door->lock();
  std::this_thread::sleep_for(std::chrono::milliseconds(600));
  ASSERT_EQ(exception, nullptr);
}
