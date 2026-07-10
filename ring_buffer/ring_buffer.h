#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>

namespace ring_buffer {

template <typename T> class RingBuffer {
public:
  // 构造函数，capacity 为缓冲区容量
  explicit RingBuffer(size_t capacity) : data_(capacity) {}

  // 禁止拷贝
  RingBuffer(const RingBuffer &) = delete;
  RingBuffer &operator=(const RingBuffer &) = delete;

  // 允许移动
  RingBuffer(RingBuffer &&) noexcept = default;
  RingBuffer &operator=(RingBuffer &&) noexcept = default;

  // 向缓冲区写入一个元素（如果已满则覆盖最旧的）
  void push(T value) {
    const uint64_t cap = static_cast<uint64_t>(data_.size());
    if (cap == 0) {
      return;
    }

    data_[tail_] = std::move(value);
    tail_ = (tail_ + 1) % cap;

    if (cnt_ == cap) {
      head_ = (head_ + 1) % cap;
    } else {
      ++cnt_;
    }
  }

  // 读取最近写入的第 n 个元素（0 = 最新，1 = 次新，...）
  // 如果 n 超出已写入的数量，返回 std::nullopt
  std::optional<T> read(size_t n) const {
    if (n >= cnt_) {
      return std::nullopt;
    }

    const uint64_t cap = static_cast<uint64_t>(data_.size());
    const uint64_t index = (tail_ + cap - 1 - static_cast<uint64_t>(n)) % cap;
    return data_[index];
  }

  // 获取当前缓冲区中有效元素数量
  size_t size() const { return static_cast<size_t>(cnt_); }

  // 获取缓冲区容量
  size_t capacity() const { return data_.size(); }

  // 清空缓冲区
  void clear() {
    head_ = 0;
    tail_ = 0;
    cnt_ = 0;
  }

  // 获取所有有效元素，从旧到新
  std::vector<T> snapshot() const {
    std::vector<T> result;
    result.reserve(size());

    const uint64_t cap = static_cast<uint64_t>(data_.size());
    for (uint64_t i = 0; i < cnt_; ++i) {
      result.push_back(data_[(head_ + i) % cap]);
    }

    return result;
  }

private:
  std::vector<T> data_;
  uint64_t head_ = 0;
  uint64_t tail_ = 0;
  uint64_t cnt_ = 0;
};

} // namespace ring_buffer