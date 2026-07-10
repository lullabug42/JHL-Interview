#pragma once

namespace timing_wheel {

void TestBasic();
void TestExactDelay();
void TestMultipleTasksSameSlot();
void TestCancelTask();
void TestLongDelayWrapAround();
void TestEmptyTick();

inline void RunTimingWheelTests() {
  TestBasic();
  TestExactDelay();
  TestMultipleTasksSameSlot();
  TestCancelTask();
  TestLongDelayWrapAround();
  TestEmptyTick();
}

} // namespace timing_wheel
