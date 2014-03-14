#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

class Display {
public:
  void clear();
  void set(unsigned x, unsigned y, int color); // 1 mark, 0 space
};

void Display::clear() {
  puts("\x1b[2J");
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

Input in;
unsigned x, y;
Display d;

void setup() {
  system("stty cbreak -echo");
  puts("\x1b[?25l");
  x = 10; y = 10;
}

int dx[] = {0,  0, 0, 1, -1};
int dy[] = {0, -1, 1, 0,  0};

void loop() {
  Direction dir = in.get();

  if (dir != NONE) {
    d.set(x, y, 0);
    x += dx[dir];
    y += dy[dir];
    d.set(x, y, 1);
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
