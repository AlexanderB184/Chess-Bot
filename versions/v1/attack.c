#include "attack.h"

#include "move_generation.h"

int can_be_attacked_by(const Board* board, Piece_Colour attacker_colour,
                       int from) {
  int row = rank_of(from), col = file_of(from);
  if (attacker_colour == board->pieces[from].colour) {
    return 0;
  }
  // Knight moves
  int offsets[8][2] = {{1, 2}, {-1, 2}, {1, -2}, {-1, -2},
                       {2, 1}, {2, -1}, {-2, 1}, {-2, -1}};
  for (size_t i = 0; i < 8; i++) {
    int attacker_row = row + offsets[i][0], attacker_col = col + offsets[i][1];
    if (attacker_row >= 0 && attacker_row < 8 && attacker_col >= 0 &&
        attacker_col < 8 &&
        board->pieces[index_of(attacker_row, attacker_col)].colour ==
            attacker_colour &&
        board->pieces[index_of(attacker_row, attacker_col)].type == KNIGHT) {
      return 1;
    }
  }
  // King moves
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      int attacker_row = row + i, attacker_col = col + j;
      if ((i != 0 || j != 0) && attacker_row >= 0 && attacker_row < 8 &&
          attacker_col >= 0 && attacker_col < 8 &&
          board->pieces[index_of(attacker_row, attacker_col)].colour ==
              attacker_colour &&
          board->pieces[index_of(attacker_row, attacker_col)].type == KING) {
        return 1;
      }
    }
  }
  // Pawn moves
  {
    int dir = attacker_colour == WHITE ? -1 : 1;
    int attacker_row = row + dir, attacker_col = col + 1;
    if (attacker_row >= 0 && attacker_row < 8 && attacker_col >= 0 &&
        attacker_col < 8 &&
        board->pieces[index_of(attacker_row, attacker_col)].colour ==
            attacker_colour &&
        board->pieces[index_of(attacker_row, attacker_col)].type == PAWN) {
      return 1;
    }
  }
  {
    int dir = attacker_colour == WHITE ? -1 : 1;
    int attacker_row = row + dir, attacker_col = col - 1;
    if (attacker_row >= 0 && attacker_row < 8 && attacker_col >= 0 &&
        attacker_col < 8 &&
        board->pieces[index_of(attacker_row, attacker_col)].colour ==
            attacker_colour &&
        board->pieces[index_of(attacker_row, attacker_col)].type == PAWN) {
      return 1;
    }
  }
  // Sliding moves
  for (int d_row = -1; d_row <= 1; d_row++) {
    for (int d_col = -1; d_col <= 1; d_col++) {
      if (d_col == 0 && d_row == 0) {
        continue;
      }
      for (size_t i = 1; i < 8; i++) {
        int attacker_row = row + d_row * i, attacker_col = col + d_col * i;
        if (attacker_row < 0 || attacker_row >= 8 || attacker_col < 0 ||
            attacker_col >= 8 ||
            board->pieces[index_of(attacker_row, attacker_col)].colour ==
                -attacker_colour) {
          break;
        }
        if (board->pieces[index_of(attacker_row, attacker_col)].type == EMPTY) {
          continue;
        }
        if ((d_row == 0 || d_col == 0) &&
            (board->pieces[index_of(attacker_row, attacker_col)].type == ROOK ||
             board->pieces[index_of(attacker_row, attacker_col)].type ==
                 QUEEN)) {
          return 1;
        }
        if ((d_row != 0 && d_col != 0) &&
            (board->pieces[index_of(attacker_row, attacker_col)].type ==
                 BISHOP ||
             board->pieces[index_of(attacker_row, attacker_col)].type ==
                 QUEEN)) {
          return 1;
        }
        break;
      }
    }
  }
  return 0;
}

int is_check(const Board* board) {
  for (int i = 0; i < 64; i++) {
    if (board->pieces[i].colour == board->turn &&
        board->pieces[i].type == KING) {
      return can_be_attacked_by(board, -board->turn, i);
    }
  }
  return 0;
}
#include <stdio.h>
int is_check_after_move(const Board* board, Move move) {
  Board cloned_board = *board;
  make_move(&cloned_board, move);
  for (int i = 0; i < 64; i++) {
    if (cloned_board.pieces[i].colour == board->turn &&
        cloned_board.pieces[i].type == KING) {
      return can_be_attacked_by(&cloned_board, -board->turn, i);
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
  if (board->half_move_count >= 100) {
    //printf("50 move rule, half_moves: %d, full_moves: %d\n",
    //       board->half_move_count, board->move_count);
    return 1;
  }
  //
  Move move_buffer[256];
  Slice(Move) buffer = {move_buffer, 256};
  Slice(Move) moves = generate_legal_moves(buffer, board);
  int stalemate = moves.length == 0;
  return stalemate;
}