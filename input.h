#ifndef _INPUT_H
#define _INPUT_H

enum Direction {
  NONE, UP, DOWN, RIGHT, LEFT
};


class Input {
public:
  virtual Direction get() = 0;
};

#endif
