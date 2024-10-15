#include <stdio.h>

#include "chess-bot/include/iterative-deepening.h"

int find_best_move_iterative_deepening_test(chess_state_t* chess_state, move_t* best_moves, size_t* times);

static chess_state_t chess_state;

void run_position(const char* position) {
  move_t moves[32];
  size_t times_ms[32];
  load_position(&chess_state, position);
  int depth = find_best_move_iterative_deepening_test(&chess_state, moves, times_ms);
  printf("position: %s\n", position);
  for (int i = 0; i < depth; i++) {
    printf("depth: %d, time: %zu, move: %d %d %d\n", i, times_ms[i],
           get_from(moves[i]), get_to(moves[i]), get_flags(moves[i]));
  }
}

int main(int argc, char const* argv[]) {
  run_position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  run_position(
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
  run_position("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
  run_position("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
  return 0;
}
