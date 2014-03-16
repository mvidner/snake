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
  int x, y;
  Position operator + (Direction dir);
  bool operator == (const Position & other) const;
};

Position Position::operator +(Direction dir) {
  Position result;
  result.x = x + dx[dir];
  result.y = y + dy[dir];
  return result;
}

bool Position::operator == (const Position & other) const {
  return x == other.x && y == other.y;
}

class Obstacle {
public:
  Obstacle();
  typedef void (*Callback)(const Obstacle &);
  virtual bool is_hit(const Position& pos) = 0;
  void on_hit(Callback);
  // "if is_hit, callback"
  void test_hit(const Position& pos);
private:
  Callback callback;
};

Obstacle::Obstacle()
  : callback(NULL)
{
}

void Obstacle::on_hit(Callback c) {
  callback = c;
}

void Obstacle::test_hit(const Position& pos) {
  if (is_hit(pos) && callback != NULL) {
    callback(*this);
  }
}

class Fence : public Obstacle {
public:
  virtual bool is_hit(const Position& pos);
};

bool Fence::is_hit(const Position& pos) {
  return pos.x < 0 || pos.y < 0 || pos.x > 50 || pos.y > 30;
}

class Snake : public Obstacle {
public:
  Snake(Display& d);

  void grow();
  // Draw the whole snake
  void draw();
  // Erase the whole snake
  void erase();
  // one step in a direction
  void step(Direction);

  // react to a command
  void input(Direction);
  // do a step in the current direction
  void step();
  Position head();
  virtual bool is_hit(const Position& pos);
private:
  void draw(bool on);
  void move();

  Display &display;
  static const int MAX = 1000;
  Position body[MAX];
  unsigned size;
  Direction dir;
  unsigned counter;
};

Snake::Snake(Display &d)
  : display(d)
{
  size = 1;
  body[0].x = 10;
  body[0].y = 10;
  dir = LEFT;
  counter = 0;
}

Position Snake::head() {
 return body[0];
}

void Snake::input(Direction d) {
  if (d != NONE) {
    dir = d;
  }
}

void Snake::step() {
  erase();
  if (counter++ % 10 == 0)
    grow();
  move();
  draw();
}

void Snake::move() {
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

bool Snake::is_hit(const Position& pos) {
  // head does not collide with itself (but with another snake??)
  for(unsigned i = 1; i < size; ++i) {
    if (pos == body[i])
      return true;
  }
  return false;
}

Input in;
VT100Display d;
//DebugDisplay d;
Snake snake(d);
Fence fence;

Obstacle* obstacles[] = { &snake, &fence };

void sighandler(int signal) {
  d.sane();
  exit(0);
}

void fence_hit(const Obstacle &) {
  puts("AAAH\n");
  sighandler(0);
}

void snake_hit(const Obstacle &) {
  puts("OUCH\n");
  sighandler(0);
}

void setup() {
  signal(SIGINT, sighandler);
  fence.on_hit(fence_hit);
  snake.on_hit(snake_hit);
  snake.draw();
}

void loop() {
  Direction dir = in.get();
  snake.input(dir);
  snake.step();
  Position head = snake.head();
  for (unsigned i = 0; i < sizeof(obstacles)/sizeof(obstacles[0]); ++i) {
    obstacles[i]->test_hit(head);
  }

  usleep(100000);
}

int main() {
  setup();
  while(1) {
    loop();
  }
  return 0;
}
