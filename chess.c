#include "chess.h"
#include "bots/randbot.h"
#include "bots/minmax.h"
#include <time.h>
int main(int argc, char const* argv[]) {
  srand(time(NULL));
  Board board = start_board;
  fprintf(stdout, "\033[s");
  
  while (!is_stalemate(&board)) {
    //display_board(stdout, &board, board.turn);
    Move move  = best_move(&board);//= get_legal_move(stdin, &board);
    make_move(&board, move);
    if (is_stalemate(&board)) {
      break;
    }
    //display_board(stdout, &board, board.turn);
    move = best_move_old(&board);
    make_move(&board, move);
  }
  //display_board(stdout, &board, board.turn);

  Piece_Colour winner = NONE;

  if (is_check(&board)) {
    winner = -board.turn;
  }
  switch (winner) {
    case NONE:
      fprintf(stdout, "stalemate!\n");
      break;
    case WHITE:
      fprintf(stdout, "white wins!\n");
      break;
    case BLACK:
      fprintf(stdout, "black wins!\n");
      break;
  }
  return 0;
}
