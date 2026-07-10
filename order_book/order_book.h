#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <unordered_map>

namespace order_book {

enum class Side {
  kAsk, // 卖盘，价格升序
  kBid, // 买盘，价格降序
};

struct Order {
  Side side;
  uint64_t price_cents = 0;
  uint64_t quantity = 0;
};

class OrderBook {
public:
  OrderBook() = default;
  ~OrderBook() = default;

  // 约定 order_id 是唯一的

  // 买入限价单
  void buyLimitOrder(uint64_t order_id, uint64_t price_cents,
                     uint64_t quantity);
  // 卖出限价单
  void sellLimitOrder(uint64_t order_id, uint64_t price_cents,
                      uint64_t quantity);
  // 撤单
  void cancelOrder(uint64_t order_id);
  // 打印当前盘口
  void printOrderBook() const;
  // 计算指定方向的加权平均价，如果没有足够的档位，打印输出 "N/A"
  void vwap(Side side, uint64_t depth) const;

private:
  struct PriceLevelInfo {
    uint64_t total_quantity = 0;
    uint64_t order_count = 0;
  };

  std::map<uint64_t, PriceLevelInfo, std::less<uint64_t>> asks_;
  std::map<uint64_t, PriceLevelInfo, std::greater<uint64_t>> bids_;
  std::unordered_map<uint64_t, Order> orders_;
};

void handleCommands(const std::string &commands, OrderBook &order_book);

} // namespace order_book
