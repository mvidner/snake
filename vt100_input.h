#ifndef _VT100_INPUT_H
#define _VT100_INPUT_H

enum Direction {
  NONE, UP, DOWN, RIGHT, LEFT
};


class Input {
public:
  Direction get();
private:
  // returns getchar, or 0 if nothing pressed
  int get_key();
};

#endif
