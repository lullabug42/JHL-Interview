#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace timing_wheel {

enum class TaskStatus {
  kPending,
  kCancelled,
};

struct Task {
  uint64_t task_id;
  std::function<void()> task;
  TaskStatus status = TaskStatus::kPending;
  uint64_t remaining_rounds = 0;
};

class TimingWheel {
public:
  // 构造函数：tick_interval 为每个 tick 的毫秒数，wheel_size 为槽数量
  explicit TimingWheel(int tick_interval_ms, int wheel_size);

  // 添加定时任务
  // delay_ms: 延迟毫秒数
  // task: 到期执行的回调函数
  // 返回: 任务 ID（用于取消）
  uint64_t addTask(int delay_ms, std::function<void()> task);

  // 取消定时任务
  // 返回: 是否成功取消
  bool cancelTask(uint64_t task_id);

  // 推进时间轮一个 tick
  // 执行所有到期任务
  void tick();

  // 获取当前已注册但未执行的任务数
  size_t pendingTaskCount() const;

private:
  int tick_interval_ms_ = 1;
  uint64_t wheel_size_ = 1;
  uint64_t current_slot_ = 0; // 已执行的最后一个槽位
  uint64_t next_task_id_ = 1;
  std::vector<std::vector<std::shared_ptr<Task>>> wheel_;
  std::unordered_map<uint64_t, std::shared_ptr<Task>> pending_tasks_;
};

} // namespace timing_wheel
