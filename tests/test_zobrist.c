#include <assert.h>

#include "../include/chess-lib.h"

zobrist_t zobrist;

static chess_state_t chess_state;

int main(int argc, char const *argv[]) {
  load_position(&chess_state, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
  zobrist_t z = zobrist;
  move_t moves[256];
  size_t move_count = generate_legal_moves(&chess_state, moves);
  for (size_t i = 0; i < move_count; i++) {
    make_move(&chess_state, moves[i]);
    assert(z != zobrist);
    unmake_move(&chess_state);
    assert(z == zobrist);
  }
  return 0;
}
