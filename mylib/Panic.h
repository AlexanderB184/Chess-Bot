#include <stdio.h>
#include <stdlib.h>

#define panic(msg, args...)                                             \
  fprintf(stderr, "panicked in file \"%s\" on line \"%d\"\n", __FILE__, \
          __LINE__);                                                    \
  fprintf(stderr, msg, ##args);                                         \
  exit(1)
