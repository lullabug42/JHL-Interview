#include "test_ring_buffer.h"
#include <iostream>

using std::cout;
using std::endl;

int main() {
  cout << "================     Ring buffer tests start     ==================" << endl;
  ring_buffer::RunRingBufferTests();
  cout << "================     Ring buffer tests end       ==================" << endl;
  return 0;
}