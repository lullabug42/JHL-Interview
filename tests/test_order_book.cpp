#include "test_order_book.h"

#include "order_book.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace order_book {

namespace {

std::string runCommandsAndCaptureOutput(const std::string &commands) {
  OrderBook order_book;
  std::ostringstream output;
  std::streambuf *original_cout = std::cout.rdbuf(output.rdbuf());

  handleCommands(commands, order_book);

  std::cout.rdbuf(original_cout);
  return output.str();
}

} // namespace

void RunOrderBookTests() {
  using std::pair;
  using std::string;
  using std::vector;

  const vector<pair<string, string>> test_cases = {
      {
          "SELL 1 10.50 100\n"
          "SELL 2 10.25 50\n"
          "BUY 3 10.00 200\n"
          "BUY 4 10.10 20\n"
          "PRINT\n",
          "ASK:\n"
          "  10.25 50 [1 orders]\n"
          "  10.50 100 [1 orders]\n"
          "\n"
          "BID:\n"
          "  10.10 20 [1 orders]\n"
          "  10.00 200 [1 orders]\n",
      },
      {
          "SELL 1 10.00 100\n"
          "SELL 2 10.00 50\n"
          "BUY 3 9.90 20\n"
          "BUY 4 9.90 30\n"
          "PRINT\n",
          "ASK:\n"
          "  10.00 150 [2 orders]\n"
          "\n"
          "BID:\n"
          "  9.90 50 [2 orders]\n",
      },
      {
          "SELL 1 10.00 100\n"
          "SELL 2 10.00 50\n"
          "BUY 3 9.90 20\n"
          "CANCEL 1\n"
          "CANCEL 999\n"
          "PRINT\n",
          "ASK:\n"
          "  10.00 50 [1 orders]\n"
          "\n"
          "BID:\n"
          "  9.90 20 [1 orders]\n",
      },
      {
          "SELL 1 10.00 100\n"
          "SELL 2 11.00 300\n"
          "BUY 3 9.00 50\n"
          "BUY 4 8.00 150\n"
          "VWAP ASK 2\n"
          "VWAP BID 2\n"
          "VWAP ASK 3\n",
          "VWAP(ASK, 2) = 10.75\n"
          "VWAP(BID, 2) = 8.25\n"
          "VWAP(ASK, 3) = N/A\n",
      },
      {
          "BUY 1 10.123 10\n"
          "SELL 2 10.456 20\n"
          "PRINT\n",
          "ASK:\n"
          "  10.46 20 [1 orders]\n"
          "\n"
          "BID:\n"
          "  10.12 10 [1 orders]\n",
      },
      {
          "BUY 1 10.00 100\n"
          "BUY 1 11.00 200\n"
          "SELL 2 12.00 50\n"
          "SELL 2 13.00 60\n"
          "PRINT\n",
          "ASK:\n"
          "  12.00 50 [1 orders]\n"
          "\n"
          "BID:\n"
          "  10.00 100 [1 orders]\n",
      },
      {
          "BUY bad 10.00 100\n"
          "SELL 1 -1.00 100\n"
          "SELL 2 10.00 0\n"
          "CANCEL 0\n"
          "VWAP UNKNOWN 1\n"
          "PRINT extra\n"
          "PRINT\n",
          "ASK:\n"
          "\n"
          "BID:\n",
      },
      {
          "PRINT\n",
          "ASK:\n"
          "\n"
          "BID:\n",
      },
      {
          "\n"
          "   \n"
          "  BUY   1   10.00   100  \n"
          "\tSELL\t2\t10.50\t50\t\n"
          "  PRINT  \n",
          "ASK:\n"
          "  10.50 50 [1 orders]\n"
          "\n"
          "BID:\n"
          "  10.00 100 [1 orders]\n",
      },
      {
          "BUY 1 10.00\n"
          "BUY 2 10.00 100 extra\n"
          "SELL 3 10.50\n"
          "SELL 4 10.50 50 extra\n"
          "CANCEL\n"
          "CANCEL 1 extra\n"
          "VWAP ASK\n"
          "VWAP ASK 1 extra\n"
          "PRINT\n",
          "ASK:\n"
          "\n"
          "BID:\n",
      },
      {
          "BUY -1 10.00 100\n"
          "BUY 18446744073709551616 10.00 100\n"
          "BUY 1 10.00 -100\n"
          "BUY 2 10.00 18446744073709551616\n"
          "BUY 3 10.00abc 100\n"
          "SELL +4 10.50 50\n"
          "SELL 5 abc 50\n"
          "PRINT\n",
          "ASK:\n"
          "\n"
          "BID:\n",
      },
      {
          "BUY 1 0.00 100\n"
          "SELL 2 0.01 50\n"
          "VWAP BID 1\n"
          "VWAP ASK 1\n"
          "PRINT\n",
          "VWAP(BID, 1) = 0.00\n"
          "VWAP(ASK, 1) = 0.01\n"
          "ASK:\n"
          "  0.01 50 [1 orders]\n"
          "\n"
          "BID:\n"
          "  0.00 100 [1 orders]\n",
      },
      {
          "SELL 1 10.00 100\n"
          "SELL 2 10.00 50\n"
          "SELL 3 10.50 10\n"
          "BUY 4 9.50 20\n"
          "BUY 5 9.50 30\n"
          "BUY 6 9.00 40\n"
          "CANCEL 1\n"
          "CANCEL 2\n"
          "CANCEL 2\n"
          "CANCEL 4\n"
          "CANCEL 5\n"
          "PRINT\n",
          "ASK:\n"
          "  10.50 10 [1 orders]\n"
          "\n"
          "BID:\n"
          "  9.00 40 [1 orders]\n",
      },
      {
          "SELL 1 10.00 100\n"
          "SELL 2 11.00 100\n"
          "SELL 3 12.00 1000\n"
          "BUY 4 9.00 100\n"
          "BUY 5 8.00 100\n"
          "BUY 6 7.00 1000\n"
          "VWAP ASK 1\n"
          "VWAP BID 1\n"
          "VWAP ASK 2\n"
          "VWAP BID 2\n",
          "VWAP(ASK, 1) = 10.00\n"
          "VWAP(BID, 1) = 9.00\n"
          "VWAP(ASK, 2) = 10.50\n"
          "VWAP(BID, 2) = 8.50\n",
      },
      {
          "VWAP ASK 1\n"
          "VWAP BID 1\n"
          "VWAP ASK 0\n"
          "VWAP BID zero\n",
          "VWAP(ASK, 1) = N/A\n"
          "VWAP(BID, 1) = N/A\n",
      },
  };

  for (const auto &[commands, expected_output] : test_cases) {
    assert(runCommandsAndCaptureOutput(commands) == expected_output);
  }
}

} // namespace order_book
