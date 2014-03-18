#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "vt100_input.h"

#include "vt100_display.h"
#include "debug_display.h"
class Score {
public:
  Score(Display &d): display(d), value(0) {}
  void add(int i);
  void draw();
private:
  Display& display;
  unsigned value;
};

void Score::add(int i) {
  value += i;
  draw();
}

void Score::draw() {
  display.set(3, 0, ' ');
  printf("%u", value);
}

int dx[] = {0,  0, 0, 1, -1};
int dy[] = {0, -1, 1, 0,  0};

struct Position {
  int x, y;
  Position operator + (Direction dir);
  bool operator == (const Position & other) const;
  static Position random(unsigned max_x, unsigned max_y);
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

Position Position::random(unsigned max_x, unsigned max_y) {
  Position result;
  result.x = ::random() % max_x;
  result.y = ::random() % max_y;
  return result;
}

class Obstacle {
public:
  Obstacle();
  typedef void (*Callback)(Obstacle &);
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
  Fence(Display &display);
  virtual bool is_hit(const Position& pos);
  void draw();
private:
  unsigned width, height;
  Display &display;
};

Fence::Fence(Display& d)
  : width(d.width())
  , height(d.height())
  , display(d)
{}

bool Fence::is_hit(const Position& pos) {
  return pos.x <= 0 || pos.y <= 0 || (unsigned) pos.x >= width || (unsigned) pos.y >= height;
}

void Fence::draw() {
  for (unsigned i = 0; i < width; ++i) {
    display.set(i, 0,      1);
    display.set(i, height, 1);
  }
  for (unsigned i = 0; i < height; ++i) {
    display.set(0,     i, 1);
    display.set(width, i, 1);
  }
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

class Foods : public Obstacle {
public:
  Foods(Display &display);
  virtual bool is_hit(const Position& pos);
  void draw();
  void handle_hit();
private:
  Display &display;
  static const unsigned MAX_FOOD = 100;
  Position items[MAX_FOOD];
  unsigned item_count;
  unsigned hit_index;

  Position new_position();
  void new_food();
};

Foods::Foods(Display &d)
  : display(d)
  , item_count(0)
{
  new_food();
}

bool Foods::is_hit(const Position& pos) {
  for(unsigned i = 0; i < item_count; ++i) {
    if (pos == items[i]) {
      hit_index = i;
      return true;
    }
  }
  return false;
}

void Foods::handle_hit() {
  Position p = new_position();
  items[hit_index] = p;
  display.set(p.x, p.y, '@');

  new_food();
}

void Foods::new_food() {
  if (item_count >= MAX_FOOD)
    return;
  Position p = new_position();
  items[item_count++] = p;
  display.set(p.x, p.y, '@');
}

Input in;
VT100Display d;
//DebugDisplay d;
Snake snake(d);
Fence fence(d);
Foods foods(d);
Obstacle* obstacles[] = { &snake, &fence, &foods };
Score score(d);

Position Foods::new_position() {
  Position p;

  bool p_is_free = true;
  do {
    p = Position::random(d.width(), d.height());
    for (unsigned i = 0; i < sizeof(obstacles)/sizeof(obstacles[0]); ++i) {
      if (obstacles[i]->is_hit(p)) {
	p_is_free = false;
	break;
      }
    }
  } while (!p_is_free);

  return p;
}


void sighandler(int signal) {
  d.sane();
  exit(0);
}

void fence_hit(Obstacle &) {
  puts("AAAH\n");
  sighandler(0);
}

void snake_hit(Obstacle &) {
  puts("OUCH\n");
  sighandler(0);
}

void foods_hit(Obstacle & f) {
  score.add(1);
  static_cast<Foods &>(f).handle_hit();
}

void setup() {
  signal(SIGINT, sighandler);
  fence.on_hit(fence_hit);
  fence.draw();
  snake.on_hit(snake_hit);
  snake.draw();
  foods.on_hit(foods_hit);
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
