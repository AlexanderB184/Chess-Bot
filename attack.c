#include "attack.h"
#include "move_generation.h"


int can_be_attacked_by(const Board* board, Piece_Colour attacker_colour,
                       int row, int col) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (board->pieces[i][j].colour == attacker_colour &&
          board->pieces[i][j].type == KING && abs(i - row) <= 1 &&
          abs(j - col) <= 1) {
      } else if (board->pieces[i][j].type != KING &&
                 board->pieces[i][j].colour == attacker_colour &&
                 is_psuedo_legal_move(board, (Move){.piece_start_row = i,
                                                    .piece_start_col = j,
                                                    .piece_end_row = row,
                                                    .piece_end_col = col})) {
        return 1;
      }
    }
  }
  return 0;
}

int is_check(const Board* board) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (board->pieces[i][j].colour == board->turn &&
          board->pieces[i][j].type == KING) {
        return can_be_attacked_by(board, -board->turn, i, j);
      }
    }
  }
  return 0;
}
#include <stdio.h>
int is_check_after_move(const Board* board, Move move) {
  Board cloned_board = *board;
  make_move(&cloned_board, move);
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (cloned_board.pieces[i][j].colour == board->turn &&
          cloned_board.pieces[i][j].type == KING) {
        return can_be_attacked_by(&cloned_board, -board->turn, i, j);
      }
    }
  }
  return 0;
}

int is_checkmate(const Board* board) {
  return is_check(board) && is_stalemate(board);
}
#include <stdio.h>
int is_stalemate(const Board* board) {
  // 50 move rule
  if (board->half_move_count >= 50) {
    printf("50 move rule, half_moves: %d, full_moves: %d\n", board->half_move_count, board->move_count);
    return 1;
  }
  //
  Scratch_Arena scratch = get_scratch(NULL, 0);
  Slice(Move) moves = generate_legal_moves(scratch.arena, board);
  int stalemate = moves.length == 0;
  release_scratch(scratch);
  return stalemate;
}