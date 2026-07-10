## 构建命令

```bash
cmake -B build -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && cmake --build build -j4
```

## 运行测试

```bash
./build/run_tests
./build/run_order_book < order_book_input.txt # 运行 OrderBook 标准输入示例
```