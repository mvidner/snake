#include "nds_input.h"

#include <nds.h>
#include <nds/arm9/input.h>

Direction NDSInput::get() {
  scanKeys();
  uint32 keys = keysCurrent();
  if (keys & KEY_LEFT)
    return LEFT;
  if (keys & KEY_RIGHT)
    return RIGHT;
  if (keys & KEY_UP)
    return UP;
  if (keys & KEY_DOWN)
    return DOWN;
  return NONE;
}

