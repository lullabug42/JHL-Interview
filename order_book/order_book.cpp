#include "order_book.h"

#include <cctype>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

namespace order_book {

namespace {

std::string formatYuan(long double price_cents, int precision) {
  std::ostringstream out;
  out << std::fixed << std::setprecision(precision) << price_cents / 100.0L;
  return out.str();
}

bool hasExtraToken(std::istringstream &input) {
  std::string extra;
  return static_cast<bool>(input >> extra);
}

bool parsePositiveUInt(const std::string &token, uint64_t &value) {
  if (token.empty()) {
    return false;
  }

  uint64_t result = 0;
  for (unsigned char ch : token) {
    if (!std::isdigit(ch)) {
      return false;
    }

    const uint64_t digit = ch - '0';
    if (result > (std::numeric_limits<uint64_t>::max() - digit) / 10) {
      return false;
    }
    result = result * 10 + digit;
  }

  if (result == 0) {
    return false;
  }

  value = result;
  return true;
}

bool parsePriceCents(const std::string &token, uint64_t &price_cents) {
  std::istringstream input(token);
  long double price_yuan = 0.0L;
  if (!(input >> price_yuan) || hasExtraToken(input) || price_yuan < 0.0L) {
    return false;
  }

  const long double cents = std::round(price_yuan * 100.0L);
  if (cents < 0.0L ||
      cents > static_cast<long double>(std::numeric_limits<uint64_t>::max())) {
    return false;
  }

  price_cents = static_cast<uint64_t>(cents);
  return true;
}

void handleCommandLine(const std::string &command, OrderBook &order_book) {
  std::istringstream input(command);
  std::string command_type;
  if (!(input >> command_type)) {
    return;
  }

  if (command_type == "BUY" || command_type == "SELL") {
    std::string order_id_token;
    std::string price_token;
    std::string quantity_token;
    if (!(input >> order_id_token >> price_token >> quantity_token) ||
        hasExtraToken(input)) {
      return;
    }

    uint64_t order_id = 0;
    uint64_t price_cents = 0;
    uint64_t quantity = 0;
    if (!parsePositiveUInt(order_id_token, order_id) ||
        !parsePriceCents(price_token, price_cents) ||
        !parsePositiveUInt(quantity_token, quantity)) {
      return;
    }

    if (command_type == "BUY") {
      order_book.buyLimitOrder(order_id, price_cents, quantity);
    } else {
      order_book.sellLimitOrder(order_id, price_cents, quantity);
    }
    return;
  }

  if (command_type == "CANCEL") {
    std::string order_id_token;
    if (!(input >> order_id_token) || hasExtraToken(input)) {
      return;
    }

    uint64_t order_id = 0;
    if (!parsePositiveUInt(order_id_token, order_id)) {
      return;
    }

    order_book.cancelOrder(order_id);
    return;
  }

  if (command_type == "PRINT") {
    if (!hasExtraToken(input)) {
      order_book.printOrderBook();
    }
    return;
  }

  if (command_type == "VWAP") {
    std::string side_token;
    std::string depth_token;
    if (!(input >> side_token >> depth_token) || hasExtraToken(input)) {
      return;
    }

    Side side = Side::kAsk;
    if (side_token == "ASK") {
      side = Side::kAsk;
    } else if (side_token == "BID") {
      side = Side::kBid;
    } else {
      return;
    }

    uint64_t depth = 0;
    if (!parsePositiveUInt(depth_token, depth)) {
      return;
    }

    order_book.vwap(side, depth);
  }
}

} // namespace

void OrderBook::buyLimitOrder(uint64_t order_id, uint64_t price_cents,
                              uint64_t quantity) {
  const auto insert_result =
      orders_.emplace(order_id, Order{Side::kBid, price_cents, quantity});
  if (!insert_result.second) {
    return;
  }

  auto &level = bids_[price_cents];
  level.total_quantity += quantity;
  ++level.order_count;
}

void OrderBook::sellLimitOrder(uint64_t order_id, uint64_t price_cents,
                               uint64_t quantity) {
  const auto insert_result =
      orders_.emplace(order_id, Order{Side::kAsk, price_cents, quantity});
  if (!insert_result.second) {
    return;
  }

  auto &level = asks_[price_cents];
  level.total_quantity += quantity;
  ++level.order_count;
}

void OrderBook::cancelOrder(uint64_t order_id) {
  const auto order_it = orders_.find(order_id);
  if (order_it == orders_.end()) {
    return;
  }

  const Order order = order_it->second;
  if (order.side == Side::kAsk) {
    auto level_it = asks_.find(order.price_cents);
    if (level_it != asks_.end()) {
      level_it->second.total_quantity -= order.quantity;
      --level_it->second.order_count;
      if (level_it->second.order_count == 0) {
        asks_.erase(level_it);
      }
    }
  } else {
    auto level_it = bids_.find(order.price_cents);
    if (level_it != bids_.end()) {
      level_it->second.total_quantity -= order.quantity;
      --level_it->second.order_count;
      if (level_it->second.order_count == 0) {
        bids_.erase(level_it);
      }
    }
  }

  orders_.erase(order_it);
}

void OrderBook::printOrderBook() const {
  std::cout << "ASK:\n";
  for (const auto &[price, level] : asks_) {
    std::cout << "  " << formatYuan(price, 2) << ' ' << level.total_quantity
              << " [" << level.order_count << " orders]\n";
  }

  std::cout << "\nBID:\n";
  for (const auto &[price, level] : bids_) {
    std::cout << "  " << formatYuan(price, 2) << ' ' << level.total_quantity
              << " [" << level.order_count << " orders]\n";
  }
}

void OrderBook::vwap(Side side, uint64_t depth) const {
  const char *side_name = side == Side::kAsk ? "ASK" : "BID";
  uint64_t total_quantity = 0;
  long double weighted_sum = 0.0L;
  uint64_t level_count = 0;

  if (side == Side::kAsk) {
    for (const auto &[price, level] : asks_) {
      if (level_count == depth) {
        break;
      }

      total_quantity += level.total_quantity;
      weighted_sum += static_cast<long double>(price) * level.total_quantity;
      ++level_count;
    }
  } else {
    for (const auto &[price, level] : bids_) {
      if (level_count == depth) {
        break;
      }

      total_quantity += level.total_quantity;
      weighted_sum += static_cast<long double>(price) * level.total_quantity;
      ++level_count;
    }
  }

  std::cout << "VWAP(" << side_name << ", " << depth << ") = ";
  if (depth == 0 || level_count < depth || total_quantity == 0) {
    std::cout << "N/A\n";
    return;
  }

  std::cout << formatYuan(weighted_sum / total_quantity, 4) << '\n';
}

void handleCommands(const std::string &commands, OrderBook &order_book) {
  std::istringstream input(commands);
  std::string command;
  while (std::getline(input, command)) {
    handleCommandLine(command, order_book);
  }
}

} // namespace order_book
