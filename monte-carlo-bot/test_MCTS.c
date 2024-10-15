#include "../chess-lib/include/chess-lib.h"
#include "include/monte-carlo-bot.h"
#include <stdio.h>
chess_state_t game = {0};

int main(int argc, char const *argv[]) {
  load_start_position(&game);

  move_t move = find_best_move(&game);
  char buffer[256];
  write_algebraic_notation(buffer, 256, &game, move);
  printf("%s\n",buffer);
  return 0;
}

// todo make program to traverse mcts tree and see scores etc
// make program to print out display of tree topology