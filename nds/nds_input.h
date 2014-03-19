#ifndef _NDS_INPUT_H
#define _NDS_INPUT_H

#include "input.h"

class NDSInput : public Input {
public:
  virtual Direction get();
};

#endif
