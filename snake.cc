#include <stdio.h>
#include <stdlib.h>
class Display {
public:
  void clear();
  void set(unsigned x, unsigned y, int color); // 1 mark, 0 space
};

enum Direction {
  NONE, UP, DOWN, LEFT, RIGHT
};

class Input {
public:
  Direction get();
};

class Score {
public:
  void add(int i);
  void display();
private:
  Display& d;
};

struct Position {
  unsigned x, y;
};

class Obstacle {
public:
  virtual bool is_hit(const Position& pos);
  void on_hit( void (*)(const Obstacle *) );
};

class Fence : public Obstacle {
public:
  virtual bool is_hit(const Position& pos);
};

class Snake {
public:
};

void teardown() {
  system("stty sane");
  printf("stty sane now\n");
}

void setup() {
  system("stty cbreak");
  atexit(teardown);
}

void loop() {
  int c = getchar();
  printf("'%c' %d 0x%02x\n", c, c, c);
}

int main() {
  setup();
  while(1) {
    loop();
  }
  return 0;
}
