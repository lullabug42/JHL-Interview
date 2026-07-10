#include "timing_wheel.h"

#include <algorithm>
#include <utility>

namespace timing_wheel {

TimingWheel::TimingWheel(int tick_interval_ms, int wheel_size)
    : tick_interval_ms_(std::max(1, tick_interval_ms)),
      wheel_size_(static_cast<uint64_t>(std::max(1, wheel_size))),
      wheel_(wheel_size_) {}

uint64_t TimingWheel::addTask(int delay_ms, std::function<void()> task) {
  const uint64_t task_id = next_task_id_++;
  const uint64_t normalized_delay =
      static_cast<uint64_t>(std::max(0, delay_ms));
  const uint64_t ticks = std::max<uint64_t>(
      1, (normalized_delay + tick_interval_ms_ - 1) / tick_interval_ms_);
  const uint64_t slot = (current_slot_ + ticks) % wheel_size_;

  auto timing_task = std::make_shared<Task>();
  timing_task->task_id = task_id;
  timing_task->task = std::move(task);
  timing_task->remaining_rounds = (ticks - 1) / wheel_size_;

  wheel_[slot].push_back(timing_task);
  pending_tasks_[task_id] = timing_task;

  return task_id;
}

bool TimingWheel::cancelTask(uint64_t task_id) {
  auto it = pending_tasks_.find(task_id);
  if (it == pending_tasks_.end()) {
    return false;
  }

  it->second->status = TaskStatus::kCancelled;
  pending_tasks_.erase(it);
  return true;
}

void TimingWheel::tick() {
  current_slot_ = (current_slot_ + 1) % wheel_size_;

  auto tasks = std::move(wheel_[current_slot_]);
  wheel_[current_slot_].clear();

  for (auto &task : tasks) {
    if (!task || task->status == TaskStatus::kCancelled) {
      continue;
    }

    if (task->remaining_rounds > 0) {
      --task->remaining_rounds;
      wheel_[current_slot_].push_back(std::move(task));
      continue;
    }

    pending_tasks_.erase(task->task_id);
    if (task->task) {
      task->task();
    }
  }
}

size_t TimingWheel::pendingTaskCount() const { return pending_tasks_.size(); }

} // namespace timing_wheel
