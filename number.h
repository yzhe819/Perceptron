#ifndef NUMBER_H
#define NUMBER_H

#include <stdlib.h>

static inline int clampi(int value, int min, int max) {
  if (value < min)
    return min;
  if (value > max)
    return max;
  return value;
}

static inline int rand_range(int low, int high) {
  return low + rand() % (high - low);
}

#endif