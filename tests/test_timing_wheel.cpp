#include "test_timing_wheel.h"

#include "timing_wheel.h"

#include <cassert>

namespace timing_wheel {

void TestBasic() {
  TimingWheel wheel(10, 8);
  int executed_count = 0;

  wheel.addTask(30, [&executed_count]() { ++executed_count; });
  assert(wheel.pendingTaskCount() == 1);

  wheel.tick();
  wheel.tick();
  assert(executed_count == 0);
  assert(wheel.pendingTaskCount() == 1);

  wheel.tick();
  assert(executed_count == 1);
  assert(wheel.pendingTaskCount() == 0);
}

void TestExactDelay() {
  constexpr int tick_interval_ms = 10;
  constexpr int delay_ms = 100;
  constexpr int expected_ticks = delay_ms / tick_interval_ms;

  TimingWheel wheel(tick_interval_ms, 16);
  bool executed = false;

  wheel.addTask(delay_ms, [&executed]() { executed = true; });

  for (int i = 0; i < expected_ticks - 1; ++i) {
    wheel.tick();
    assert(!executed);
    assert(wheel.pendingTaskCount() == 1);
  }

  wheel.tick();
  assert(executed);
  assert(wheel.pendingTaskCount() == 0);
}

void TestMultipleTasksSameSlot() {
  TimingWheel wheel(10, 8);
  int executed_count = 0;
  int executed_sum = 0;

  wheel.addTask(30, [&]() {
    ++executed_count;
    executed_sum += 1;
  });
  wheel.addTask(30, [&]() {
    ++executed_count;
    executed_sum += 2;
  });
  wheel.addTask(30, [&]() {
    ++executed_count;
    executed_sum += 3;
  });

  assert(wheel.pendingTaskCount() == 3);

  wheel.tick();
  wheel.tick();
  assert(executed_count == 0);
  assert(executed_sum == 0);
  assert(wheel.pendingTaskCount() == 3);

  wheel.tick();
  assert(executed_count == 3);
  assert(executed_sum == 6);
  assert(wheel.pendingTaskCount() == 0);
}

void TestCancelTask() {
  TimingWheel wheel(10, 8);
  bool executed = false;

  const uint64_t task_id =
      wheel.addTask(30, [&executed]() { executed = true; });

  assert(wheel.pendingTaskCount() == 1);
  assert(wheel.cancelTask(task_id));
  assert(wheel.pendingTaskCount() == 0);

  for (int i = 0; i < 3; ++i) {
    wheel.tick();
  }

  assert(!executed);
}

void TestLongDelayWrapAround() {
  constexpr int tick_interval_ms = 10;
  constexpr int wheel_size = 5;
  constexpr int delay_ms = 120;
  constexpr int expected_ticks = delay_ms / tick_interval_ms;

  TimingWheel wheel(tick_interval_ms, wheel_size);
  bool executed = false;

  wheel.addTask(delay_ms, [&executed]() { executed = true; });
  assert(wheel.pendingTaskCount() == 1);

  for (int i = 0; i < expected_ticks - 1; ++i) {
    wheel.tick();
    assert(!executed);
    assert(wheel.pendingTaskCount() == 1);
  }

  wheel.tick();
  assert(executed);
  assert(wheel.pendingTaskCount() == 0);
}

void TestEmptyTick() {
  TimingWheel wheel(10, 8);

  assert(wheel.pendingTaskCount() == 0);

  for (int i = 0; i < 20; ++i) {
    wheel.tick();
    assert(wheel.pendingTaskCount() == 0);
  }
}

} // namespace timing_wheel
