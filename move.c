#include "move.h"

#include "board.h"

void make_move(Board* board, Move move) {
  int start_row = move.piece_start_row;
  int start_col = move.piece_start_col;
  int end_row = move.piece_end_row;
  int end_col = move.piece_end_col;
  Piece piece = board->pieces[start_row][start_col];
  if (piece.type == PAWN) {
    board->half_move_count = 0;
  } else {
    board->half_move_count++;
  }

  // castling
  if (piece.type == KING && move.piece_start_col == 4 &&
      move.piece_end_col == 2) {
    board->pieces[end_row][3] = board->pieces[start_row][0];
    board->pieces[start_row][0] = (Piece){NONE, EMPTY};
  }
  if (piece.type == KING && move.piece_start_col == 4 &&
      move.piece_end_col == 6) {
    board->pieces[end_row][5] = board->pieces[start_row][7];
    board->pieces[start_row][7] = (Piece){NONE, EMPTY};
  }

  // enpassent
  if (piece.type == PAWN && end_col == board->enpassent_target_col &&
      start_row == board->enpassent_target_row) {
    board->pieces[board->enpassent_target_row][board->enpassent_target_col] =
        (Piece){NONE, EMPTY};
  }

  // double pawn move
  if (piece.type == PAWN && abs(end_row - start_row) == 2) {
    board->enpassent_target_row = end_row;
    board->enpassent_target_col = end_col;
  } else {
    board->enpassent_target_row = -1;
    board->enpassent_target_col = -1;
  }

  board->pieces[end_row][end_col] = board->pieces[start_row][start_col];

  board->pieces[start_row][start_col] = (Piece){NONE, EMPTY};

  // promotion
  if (piece.type == PAWN && (end_row == 0 || end_row == 7)) {
    board->pieces[end_row][end_col].type = move.promote_to;
  }

  // castle rights
  if ((start_row == 0 && start_col == 0) || (end_row == 0 && end_col == 0)) {
    board->rights &= ~WHITE_QUEEN_SIDE;
  }
  if ((start_row == 0 && start_col == 7) || (end_row == 0 && end_col == 7)) {
    board->rights &= ~WHITE_KING_SIDE;
  }
  if ((start_row == 7 && start_col == 0) || (end_row == 7 && end_col == 0)) {
    board->rights &= ~BLACK_QUEEN_SIDE;
  }
  if ((start_row == 7 && start_col == 7) || (end_row == 7 && end_col == 7)) {
    board->rights &= ~BLACK_KING_SIDE;
  }
  if ((start_row == 0 && start_col == 4) || (end_row == 0 && end_col == 4)) {
    board->rights &= ~WHITE_BOTH_SIDES;
  }
  if ((start_row == 7 && start_col == 4) || (end_row == 7 && end_col == 4)) {
    board->rights &= ~BLACK_BOTH_SIDES;
  }

  board->turn = -board->turn;
  board->move_count++;
}