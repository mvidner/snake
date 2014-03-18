#ifndef _DISPLAY_H
#define _DISPLAY_H

class Display {
public:
  Display() {}
  virtual void clear() {}
  virtual void set(unsigned x, unsigned y, int color) = 0; // 1 mark, 0 space
  virtual void sane() {}
  virtual unsigned width() = 0;
  virtual unsigned height() = 0;
};

#endif
