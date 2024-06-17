#include <math.h>
#include <stddef.h>
#include <stdio.h>

#define numbers_count 100000ull

static size_t numbers[numbers_count];

int main(int argc, char const *argv[]) {
  numbers[0] = 1;
  numbers[1] = 1;
  numbers[2] = 0;
  for (size_t p = 2; p < sqrtl(numbers_count); p++) {
    if (numbers[p] == 1) continue;
    for (size_t i = 2 * p; i < numbers_count; i += p) {
      numbers[i] = 1;
    }
  }

  printf("{");
  for (size_t i = 0; i < numbers_count; i++) {
    if (numbers[i] == 0) {
      printf("%zu,", i);
    }
  }
  printf("}\n");
  return 0;
}
