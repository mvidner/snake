#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "vt100_input.h"

#include "vt100_display.h"
#include "debug_display.h"
class Score {
public:
  Score(Display &ad): d(ad) {}
  void add(int i);
  void display();
private:
  Display& d;
};

int dx[] = {0,  0, 0, 1, -1};
int dy[] = {0, -1, 1, 0,  0};

struct Position {
  unsigned x, y;
  Position operator + (Direction dir);
};

Position Position::operator +(Direction dir) {
  Position result;
  result.x = x + dx[dir];
  result.y = y + dy[dir];
  return result;
}

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
  Snake(Display& d);

  void grow();
  // Draw the whole snake
  void draw();
  // Erase the whole snake
  void erase();
  // one step in a direction
  void step(Direction);
private:
  void draw(bool on);
  Display &display;
  static const int MAX = 1000;
  Position body[MAX];
  unsigned size;
};

Snake::Snake(Display &d)
  : display(d)
{
  size = 1;
  body[0].x = 10;
  body[0].y = 10;
}

void Snake::step(Direction dir) {
  for(unsigned i = size - 1; i > 0; --i) {
    body[i] = body[i-1];
  }
  body[0] = body[0] + dir;
}

void Snake::grow() {
  if (size < MAX) {
    ++size;
  }
}

void Snake::draw(bool on) {
  for(unsigned i = 0; i < size; ++i) {
    display.set(body[i].x, body[i].y, on? 1: 0);
  }
}

void Snake::draw() {
  draw(true);
}

void Snake::erase() {
  draw(false);
}

Input in;
VT100Display d;
//DebugDisplay d;
Snake snake(d);

void sighandler(int signal) {
  d.sane();
  exit(0);
}

void setup() {
  signal(SIGINT, sighandler);
  snake.draw();
}

void loop() {
  Direction dir = in.get();
  static unsigned counter = 0;
  if (dir != NONE) {
    snake.erase();
    if (counter++ % 10 == 0)
      snake.grow();
    snake.step(dir);
    snake.draw();
  }

  usleep(20000);
}

int main() {
  setup();
  while(1) {
    loop();
  }
  return 0;
}
