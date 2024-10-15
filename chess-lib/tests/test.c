#include "chess-bot/include/iterative-deepening.h"

int main(int argc, char const* argv[]) {
  chess_state_t chess_state = {};
  load_start_position(&chess_state);
  find_best_move(&chess_state);
  return 0;
}
