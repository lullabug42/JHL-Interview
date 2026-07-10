#include "test_ring_buffer.h"
#include "test_order_book.h"
#include "test_timing_wheel.h"
#include <iostream>

using std::cout;
using std::endl;

int main() {
  cout << "================     Ring buffer tests start     ==================" << endl;
  ring_buffer::RunRingBufferTests();
  cout << "================     Ring buffer tests end       ==================" << endl;

  cout << "================     Timing wheel tests start    ==================" << endl;
  timing_wheel::RunTimingWheelTests();
  cout << "================     Timing wheel tests end      ==================" << endl;

  cout << "================     Order book tests start      ==================" << endl;
  order_book::RunOrderBookTests();
  cout << "================     Order book tests end        ==================" << endl;
  return 0;
}
