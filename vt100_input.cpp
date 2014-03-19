#include "vt100_input.h"

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>

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

