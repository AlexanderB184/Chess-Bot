#include "move_validation.h"
#include <stddef.h>

int check_line(const Board* board, int start_row, int start_col, int rows_moved,
               int cols_moved) {
  int dr = sgn(rows_moved);
  int dc = sgn(cols_moved);
  for (size_t i = 1; i < max(abs(rows_moved), abs(cols_moved)); i++) {
    if (board->pieces[start_row + dr * i][start_col + dc * i].type != EMPTY) {
      return 1;
    }
  }
  return 0;
}

int is_legal_pawn_move(const Board* board, Move move) {
  int start_row = move.piece_start_row;
  int start_col = move.piece_start_col;
  int end_row = move.piece_end_row;
  int end_col = move.piece_end_col;
  Piece piece = board->pieces[start_row][start_col];

  int dir = piece.colour == WHITE ? 1 : -1;

  // single forward step
  if (start_col == end_col && end_row - start_row == dir &&
      (start_row == 1 || start_row == 6) &&
      board->pieces[start_row + dir][start_col].type == EMPTY) {
    return 1;
  }
  // double forward step
  if (start_col == end_col && end_row - start_row == 2 * dir &&
      (start_row == 1 || start_row == 6) &&
      board->pieces[start_row + dir][start_col].type == EMPTY &&
      board->pieces[start_row + 2 * dir][start_col].type == EMPTY) {
    return 1;
  }

  // capture and enpassent
  if (abs(move.piece_start_col - move.piece_end_col) == 1 &&
      move.piece_end_row - move.piece_start_row == dir &&
      (board->pieces[end_row][end_col].colour == -piece.colour ||
       (board->enpassent_target_col == move.piece_end_col &&
        board->enpassent_target_row == move.piece_start_row))) {
    return 1;
  }

  return 0;
}

int is_legal_knight_move(const Board* board, Move move) {
  int start_row = move.piece_start_row;
  int start_col = move.piece_start_col;
  int end_row = move.piece_end_row;
  int end_col = move.piece_end_col;
  if (abs(start_row - end_row) > 2 || abs(start_col - end_col) > 2 ||
      abs(start_col - end_col) + abs(start_row - end_row) != 3) {
    return 0;
  }
  return 1;
}

int is_legal_bishop_move(const Board* board, Move move) {
  int start_row = move.piece_start_row;
  int start_col = move.piece_start_col;
  int end_row = move.piece_end_row;
  int end_col = move.piece_end_col;
  int cols_moved = end_col - start_col;
  int rows_moved = end_row - start_row;
  if (abs(cols_moved) != abs(rows_moved) ||
      check_line(board, start_row, start_col, rows_moved, cols_moved)) {
    return 0;
  }
  return 1;
}

int is_legal_rook_move(const Board* board, Move move) {
  int start_row = move.piece_start_row;
  int start_col = move.piece_start_col;
  int end_row = move.piece_end_row;
  int end_col = move.piece_end_col;
  int cols_moved = end_col - start_col;
  int rows_moved = end_row - start_row;
  if ((cols_moved != 0 && rows_moved != 0) ||
      check_line(board, start_row, start_col, rows_moved, cols_moved)) {
    return 0;
  }
  return 1;
}

int is_legal_queen_move(const Board* board, Move move) {
  int start_row = move.piece_start_row;
  int start_col = move.piece_start_col;
  int end_row = move.piece_end_row;
  int end_col = move.piece_end_col;
  int cols_moved = end_col - start_col;
  int rows_moved = end_row - start_row;
  if ((abs(rows_moved) != abs(cols_moved) && cols_moved != 0 &&
       rows_moved != 0) ||
      check_line(board, start_row, start_col, rows_moved, cols_moved)) {
    return 0;
  }
  return 1;
}

int is_legal_king_move(const Board* board, Move move) {
  int start_row = move.piece_start_row;
  int start_col = move.piece_start_col;
  int end_row = move.piece_end_row;
  int end_col = move.piece_end_col;
  Piece piece = board->pieces[start_row][start_col];

  // standard king move
  if ((abs(start_row - end_row) < 2 && abs(start_col - end_col) < 2)) {
    return 1;
  }

  // if king in check then cannot castle
  if (can_be_attacked_by(board, -board->turn, start_row, start_col)) {
    return 0;
  }

  // queen side castle
  if (!can_be_attacked_by(board, -board->turn, start_row, 3) &&
      !can_be_attacked_by(board, -board->turn, start_row, 2) &&
      board->pieces[start_row][3].type == EMPTY &&
      board->pieces[start_row][2].type == EMPTY &&
      board->pieces[start_row][1].type == EMPTY && 
      start_col == 4 && end_col == 2 &&
      ((piece.colour == WHITE && board->rights & WHITE_QUEEN_SIDE) ||
       (piece.colour == BLACK && board->rights & BLACK_QUEEN_SIDE))) {
    return 1;
  }

  // king side castle
  if (!can_be_attacked_by(board, -board->turn, start_row, 5) &&
      !can_be_attacked_by(board, -board->turn, start_row, 6) &&
      board->pieces[start_row][5].type == EMPTY &&
      board->pieces[start_row][6].type == EMPTY && 
      start_col == 4 && end_col == 6 &&
      ((piece.colour == WHITE && board->rights & WHITE_KING_SIDE) ||
       (piece.colour == BLACK && board->rights & BLACK_KING_SIDE))) {
    return 1;
  }

  return 0;
}

int is_psuedo_legal_move(const Board* board, Move move) {
  int start_row = move.piece_start_row;
  int start_col = move.piece_start_col;
  int end_row = move.piece_end_row;
  int end_col = move.piece_end_col;
  Piece piece = board->pieces[start_row][start_col];

  // null move
  if (start_col == end_col && start_row == end_row) {
    return 0;
  }

  // trying to capture own piece
  if (piece.colour == board->pieces[end_row][end_col].colour) {
    return 0;
  }
  // check movement follows rules of chess
  switch (piece.type) {
    case EMPTY:
      return 0;
    case PAWN:
      return is_legal_pawn_move(board, move);
    case KNIGHT:
      return is_legal_knight_move(board, move);
    case BISHOP:
      return is_legal_bishop_move(board, move);
    case ROOK:
      return is_legal_rook_move(board, move);
    case QUEEN:
      return is_legal_queen_move(board, move);
    case KING:
      return is_legal_king_move(board, move);
  }
  return 0;
}

int is_legal_move(const Board* board, Move move) {
  // out of bounds movement
  if (move.piece_start_col < 0 || move.piece_end_col < 0 ||
      move.piece_start_row < 0 || move.piece_end_row < 0 ||
      move.piece_start_col >= 8 || move.piece_end_col >= 8 ||
      move.piece_start_row >= 8 || move.piece_end_row >= 8) {
    return 0;
  }

  // trying to move piece whose turn it isn't
  if (board->pieces[move.piece_start_row][move.piece_start_col].colour !=
      board->turn) {
    return 0;
  }

  // check move follows chess rules
  if (!is_psuedo_legal_move(board, move)) {
    return 0;
  }

  // no moving into check
  if (is_check_after_move(board, move)) {
    return 0;
  }

  return 1;
}
