#ifndef _VT100_DISPLAY_H
#define _VT100_DISPLAY_H

#include "display.h"

class VT100Display: public Display {
public:
  VT100Display();
  virtual void clear();
  virtual void set(unsigned x, unsigned y, int color); // 1 mark, 0 space
  virtual void sane();
  virtual unsigned width();
  virtual unsigned height();
 private:
  unsigned width_, height_;
};

#endif
