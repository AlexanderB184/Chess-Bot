#include <time.h>
#include <stdio.h>

#include "../include/chess-lib.h"
#include "../include/perft.h"

static chess_state_t chess_state;

int main(int argc, char const* argv[]) {
  load_position(&chess_state, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  time_t start = time(NULL);
  perft(&chess_state, 6, PERFT_JUST_NODES);
  time_t end = time(NULL);
  time_t time_taken = end - start;
  printf("time taken: %zu seconds\n", time_taken);
  return 0;
}