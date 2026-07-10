## 1. 使用概览

本项目使用 codex 辅助开发，LLM 为 GPT-5.5.

分为两个 session. 一个 session 用于业务代码的编写，另一个 session 用于测试用例的编写。业务代码的编写完成后，再统一进行测试用例的编写。

## 2. 业务代码编写交互内容

### 2.1 Ring Buffer

已创建 ring_buffer/ring_buffer.h 与 ring_buffer/CMakeLists.txt 文件，ring_buffer.h 包含 RingBuffer 模板声明。

```text
Q1: 实现 [ring_buffer.h](ring_buffer/ring_buffer.h) 的环形缓冲区，环形缓冲区底层存储泛型 T 使用 std::vector<T>, vector 长度为 capacity; 使用类型为 uint64_t 的 head_ / tail_ / cnt_ 分别记录 环形缓冲区最旧的元素位置数组下标索引 / 环形缓冲区下一次写入位置的数组下标索引 / 环形缓冲区内已缓冲的元素数量
```

A1 摘要：

完成了 ring_buffer.h header-only lib 的实现，运行了项目构建，并使用临时断言程序验证了多种情况。

### 2.2 Timing Wheel

已创建 timing_wheel/timing_wheel.h, timing_wheel/timing_wheel.cpp, timing_wheel/CMakeLists.txt 文件，timing_wheel.h 包含 `TimingWheel`, `TaskStatus`, `Task` 的声明。

```text
Q2: 完成 [timing_wheel](timing_wheel/) 中 TimingWheel 类的类方法定义，TimingWheel 使用 std::vector<std::vector<std::shared_ptr<Task>>> 作为时间轮数据结构，使用 std::unordered_map<uint64_t, std::shared_ptr<Task>> 存储 task_id 与 未执行 Task 的映射, Task 代表一个定时任务，TaskStatus 为 Task 的状态，在取消定时任务时，status 变为 kCancelled, task_id 使用自增 id; 若延迟超过时间环一轮的时间，在 Task remaining_rounds field 中记录应在多少圈后执行
不使用 throw 异常，并假设 TimingWheel 类在单线程中执行
```

A2 摘要：

完成了 timing_wheel.h 中 TimingWheel 类的类方法定义，运行了项目构建，并使用临时断言程序验证了多种情况。

### 2.3 Order Book

已创建 order_book/order_book.h, order_book/order_book.cpp, order_book/CMakeLists.txt 文件，order_book.h 包含 `Side`, `Order`, `OrderBook`, `OrderBook::PriceLevelInfo` 的声明。

```text
Q3: 完成 [order_book](order_book/) OrderBook 方法的定义

OrderBook 类使用两个 std::map<uint64_t, PriceLevelInfo, cmp> 分别存储 Ask / Bid Side 的订单，key 为价格档位（单位：分）, Ask 升序存储，Bid 降序存储，使用 std::unordered_map<uint64_t, Order> 存储价格档位与订单的映射

buyLimitOrder 方法将订单加入 Bid Side, sellLimitOrder 方法将订单加入 Ask Side

printOrderBook 方法打印格式为：

ASK :
  <price> <total_quantity> [<count> orders]

BID :
  <price> <total_quantity> [<count> orders]

ASK 价格升序打印，BID 价格降序打印
```

A3 摘要：

完成了 OrderBook 方法定义，添加参数校验保护，运行了项目构建，并使用额外临时断言程序验证了多种情况。

```text
发现 std::map compare 函数未使用标准库提供的实现，而是使用自定义的实现。
Q4: map cmp 可以直接使用 std::greater 与 std::less
```

A4 摘要：

按建议改为：OrderBook 的 map comparator 直接使用标准库比较器。

```text
对齐 VWAP 方法输出格式。
Q5: 根据 [order_book.h](order_book/order_book.h) 中 vwap 的新声明，修改 [order_book.cpp](order_book/order_book.cpp) 中定义，输出格式为

VWAP(side, depth) = <result>

side 为 "ASK" 或 "BID"
```

A5 摘要：

按建议修改了 VWAP 方法定义，运行了项目构建，并使用额外临时断言程序验证了多种情况。


```text
格式化输出的价格为分，需要进行单位转换。
Q6: 修改[order_book.cpp](order_book/order_book.cpp) printOrderBook 与 vwap, 输出的价格数值单位为元，并保留两位小数
```

A6 摘要：

添加了 helper 函数，将价格从分转换为元，并保留两位小数。

```text
Q7: 完成 [order_book.h](order_book/order_book.h) 的 handleCommand

handleCommand 使用 istringstream 处理字符串，命令格式如下

BUY <order_id> <price> <quantity>    # 买入限价单
SELL <order_id> <price> <quantity>   # 卖出限价单
CANCEL <order_id>                     # 撤单
PRINT                                # 打印当前盘口
VWAP <side> <depth>                   # 计算指定方向的加权平均价

其中 order_id quantity depth 为正整数，price 为浮点数，side 为 ASK 或 BID 字符串
```

A7 摘要：

完成 order_book.h 中声明的 handleCommand，实现位于 order_book.cpp

```text
考虑到多行指令输入的情况
Q8: handleCommands 声明已更新， commands 可能包含多行指令，请修改 handleCommands 定义以适配多行 cmds 的输入
```

A8 摘要：

新增内部 helper 用于处理单行命令，并使用 getline 逐行读取多行指令。

```text
修正 vwap 方法精度为小数后四位
Q9: [order_book.cpp](order_book/order_book.cpp) vwap 小数精度修改为 4 位
```

A9 摘要：

按建议修改了 vwap 方法精度为小数后四位。

```text
新增从标准输入读取指令的方法。
Q10: [order_book.h](order_book/order_book.h) 新增函数声明，并于 [order_book.cpp](order_book/order_book.cpp) 中实现

函数循环读取标准输入，内部直接调用 handleCommands 对输入进行解析，并且当输入为 EXIT 时，跳出循环
```

A10 摘要：

完成了所需的方法。

## 3. 测试用例编写交互内容

新建 tests/CMakeLists.txt, ring_buffer 测试直接使用文档中的测试用例。

新建 tests/test_timing_wheel.cpp, tests/test_timing_wheel.h, 配置对应 cmake target.

```text
Q1: [tests](tests/) 中编写 timing_wheel 的测试用例，需要编写测试覆盖以下场景

1. **基本功能**：添加任务，推进足够的 tick 后，任务被执行
2. **精确延迟**：延迟 100ms 的任务在恰好 `100 / tick_interval` 个 tick 后执行
3. **多任务同槽**：多个任务落在同一个槽位，都能执行
4. **取消任务**：添加后取消，推进 tick 后不执行
5. **超长延迟**：delay 超过一圈（wrap around），任务仍能正确执行
6. **空 tick**：没有到期任务时，tick 正常推进不崩溃

判断使用 assert, 每一项场景使用单独的函数声明，并且将函数声明添加到 RunTimingWheelTests 中
```

A1 摘要：

补齐了 timing_wheel 的 6 个独立测试场景，并全部加入 RunTimingWheelTests().

```text
Q2: [tests](tests/) 中编写 [order_book](order_book/) OrderBook 的测试，测试用例使用 std::vector<std::pair<string, string>> 存储，pair.first 为输入的 commends, pair.second 为预期输出的结果
```

A2 摘要：

新增 OrderBook 测试，并按要求使用 std::vector<std::pair<string, string>> 存储，pair.first 为输入的 commends, pair.second 为预期输出的结果。

```text
Q3: 检验 [test_order_book.cpp](tests/test_order_book.cpp) 是否覆盖了所有的指令，并思考可能会出现哪些边缘情况，并进行测试用例的扩展

指令列表如下

BUY <order_id> <price> <quantity>    # 买入限价单
SELL <order_id> <price> <quantity>   # 卖出限价单
CANCEL <order_id>                     # 撤单
PRINT                                # 打印当前盘口
VWAP <side> <depth>                   # 计算指定方向的加权平均价
```

A3 摘要：

检查并扩展了 OrderBook 测试用例，覆盖了所有的指令，并思考可能会出现哪些边缘情况，并进行测试用例的扩展。

```text
修正 vwap 方法精度为小数后四位
Q4: [test_order_book.cpp](tests/test_order_book.cpp) 对测试用例进行修正，vwap 的输出小数精度修正为 4 位
```

A4 摘要：

修正 tests/test_order_book.cpp 中所有 VWAP 数值输出的预期精度，从 2 位小数改为 4 位小数。

```text
Q5: [test_ring_buffer.cpp](tests/test_ring_buffer.cpp) 添加 snap 的断言
```

A5 摘要：

添加了 snap 的断言。
