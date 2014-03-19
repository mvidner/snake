#ifndef _VT100_INPUT_H
#define _VT100_INPUT_H

#include "input.h"

class VT100Input : public Input {
public:
  virtual Direction get();
private:
  // returns getchar, or 0 if nothing pressed
  int get_key();
};

#endif
