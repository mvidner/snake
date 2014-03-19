#include "debug_display.h"

#include <stdio.h>

DebugDisplay::DebugDisplay() {
  puts("Debug Display\n");
}

void DebugDisplay::clear() {
}

//void DebugDisplay::sane() {
//}

void DebugDisplay::set(unsigned x, unsigned y, int color) {
  printf("[%u, %u] %d\n", y, x, color);
}
