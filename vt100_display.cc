#include "vt100_display.h"

#include <stdio.h>
#include <stdlib.h>

VT100Display::VT100Display() {
  system("stty cbreak -echo");
  puts("\x1b[?25l"); // hide cursor
}

void VT100Display::clear() {
  puts("\x1b[2J");
}

void VT100Display::sane() {
  puts("\x1b[?25h");
  system("stty sane");
}

void VT100Display::set(unsigned x, unsigned y, int color) {
  int c = color;
  if (c == 0)
    c = ' ';
  else if (c == 1)
    c = '*';

  printf("\x1b[%u;%uH%c", y, x, c);
  fflush(stdout);
}
