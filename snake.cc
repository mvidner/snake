#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

class Display {
public:
  Display();
  virtual void clear();
  virtual void set(unsigned x, unsigned y, int color); // 1 mark, 0 space
  virtual void sane();
};

Display::Display() {
  system("stty cbreak -echo");
  puts("\x1b[?25l"); // hide cursor
}

void Display::clear() {
  puts("\x1b[2J");
}

void Display::sane() {
  puts("\x1b[?25h");
  system("stty sane");
}

void Display::set(unsigned x, unsigned y, int color) {
  int c = color;
  if (c == 0)
    c = ' ';
  else if (c == 1)
    c = '*';

  printf("\x1b[%u;%uH%c", y, x, c);
  fflush(stdout);
}

class DebugDisplay: public Display {
public:
  DebugDisplay();
  virtual void clear();
  virtual void set(unsigned x, unsigned y, int color); // 1 mark, 0 space
  //  virtual void sane();
};

DebugDisplay::DebugDisplay() {
  puts("Debug Display\n");
}

void DebugDisplay::clear() {
}

//void DebugDisplay::sane() {
//}

void DebugDisplay::set(unsigned x, unsigned y, int color) {
  printf("[%u, %u] %d\n", y, x, color);
}

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

int Input::get_key() {
  fd_set rfds;
  struct timeval tv;
  int retval;

  /* Watch stdin (fd 0) to see when it has input. */
  FD_ZERO(&rfds);
  FD_SET(0, &rfds);
  tv.tv_sec = 0;
  tv.tv_usec = 0;

  retval = select(1, &rfds, NULL, NULL, &tv);
  if (retval == -1) {
    perror("select()");
    return 0;
  }
  else if (retval) {
    return getchar();
  }
  else {
    return 0;
  }
}

Direction Input::get() {
  int key;
  if ((key = get_key()) != 27) {
    return NONE;
  }
  if ((key = getchar()) != '[') {
    return NONE;
  }
  key = getchar();
  if ('A' <= key && key <= 'D')
    return (Direction)(key - 'A' + 1);

  return NONE;
}

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
Display d;
//DebugDisplay d;
Snake snake(d);

void sighandler(int signal) {
  d.sane();
  exit(0);
}

void setup() {
  signal(SIGINT, sighandler);
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
