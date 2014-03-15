#ifndef _DEBUG_DISPLAY_H
#define _DEBUG_DISPLAY_H

#include "display.h"

class DebugDisplay: public Display {
public:
  DebugDisplay();
  virtual void clear();
  virtual void set(unsigned x, unsigned y, int color); // 1 mark, 0 space
  //  virtual void sane();
};

#endif
