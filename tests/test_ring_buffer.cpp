#include "test_ring_buffer.h"

#include "ring_buffer.h"

#include <cassert>
#include <optional>

namespace ring_buffer {

void RunRingBufferTests() {
  // 1. 基本读写
  RingBuffer<int> buf(3);
  buf.push(10);
  buf.push(20);
  buf.push(30);
  assert(buf.size() == 3);
  assert(buf.read(0) == 30); // 最新
  assert(buf.read(1) == 20); // 次新
  assert(buf.read(2) == 10); // 最旧

  // 2. 覆盖写入
  buf.push(40); // 容量 3，覆盖最旧的 10
  assert(buf.size() == 3);
  assert(buf.read(2) == 20); // 现在最旧的是 20
  assert(buf.read(0) == 40); // 最新是 40

  // 3. 越界读取
  assert(buf.read(5) == std::nullopt);

  // 4. 快照
  auto snap = buf.snapshot(); // [20, 30, 40] 从旧到新
  assert(snap.size() == 3);
  assert(snap[0] == 20);
  assert(snap[1] == 30);
  assert(snap[2] == 40);

  // 5. 清空
  buf.clear();
  assert(buf.size() == 0);
  assert(buf.read(0) == std::nullopt);
}

} // namespace ring_buffer
