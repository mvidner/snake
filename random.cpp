#include "random.h"

// from man rand(3p)
static unsigned long next = 1;
int myrand(void)  /* RAND_MAX assumed to be 32767. */
{
  next = next * 1103515245 + 12345;
  return((unsigned)(next/65536) % 32768);
}


void mysrand(unsigned seed)
{
  next = seed;
}

