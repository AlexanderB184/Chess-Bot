#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static uint64_t zobrist_table[12][64];

int main(int argc, char const *argv[]) {
  srand(time(NULL));
  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 64; j++) {
      zobrist_table[i][j] = (uint64_t)rand() | ((uint64_t)rand() << 32);
    }
  }

  fprintf(stdout, "{\n");

  for (int i = 0; i < 12; i++) {
    fprintf(stdout, "{");

    for (int j = 0; j < 64; j++) {
      fprintf(stdout, "0x%016lx, ", zobrist_table[i][j]);
    }
    fprintf(stdout, "},\n");
  }
  fprintf(stdout, "};\n");

  fprintf(stdout, "0x%0lx\n", (uint64_t)rand() | ((uint64_t)rand() << 32));
  return 0;
}
