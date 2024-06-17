#include "move_validation.h"

#include <stddef.h>

int overflows(int index, int offset, int row_diff, int col_diff) {
  return index + offset < 0 || index + offset >= 64 ||
         rank_diff(index, index + offset) / 8 != row_diff ||
         rank_diff(index, index + offset) != col_diff;
}

int check_line(const Board* board, int start_row, int start_col, int rows_moved,
               int cols_moved) {
  int dr = sgn(rows_moved);
  int dc = sgn(cols_moved);
  for (size_t i = 1; i < max(abs(rows_moved), abs(cols_moved)); i++) {
    int idx = (start_row + dr * i) * 8 + start_col + dc * i;
    if (board_get_piece(board, idx).type != EMPTY) {
      return 1;
    }
  }
  return 0;
}

int is_legal_pawn_move(const Board* board, Move move) {
  Piece piece_to_move = board_get_piece(board, move.from);
  // Piece target_square = board_get_piece(board, move.to);

  int dir = piece_to_move.colour == WHITE ? 1 : -1;

  // single forward step
  if (move.to == move.from + 8 * dir &&
      board_get_piece(board, move.to).type == EMPTY) {
    return 1;
  }
  // double forward step
  if (move.to == move.from + 16 * dir &&
      (rank_of(move.from) == 1 || rank_of(move.from) == 6) &&
      board_get_piece(board, move.from + 8 * dir).type == EMPTY &&
      board_get_piece(board, move.from + 16 * dir).type == EMPTY) {
    return 1;
  }

  // capture and enpassent
  if (rank_of(move.to) == rank_of(move.from) + dir &&
      abs(file_of(move.to) - file_of(move.from)) == 1 &&  // check for overflows
      (board_get_piece(board, move.to).colour == -piece_to_move.colour ||
       (file_of(board->enpassent) == file_of(move.to) &&
        rank_of(board->enpassent) == rank_of(move.from)))) {
    return 1;
  }

  return 0;
}

int is_legal_knight_move(const Board* board, Move move) {
  int start_row = rank_of(move.from);
  int start_col = file_of(move.from);
  int end_row = rank_of(move.to);
  int end_col = file_of(move.to);
  if (abs(start_row - end_row) > 2 || abs(start_col - end_col) > 2 ||
      abs(start_col - end_col) + abs(start_row - end_row) != 3) {
    return 0;
  }
  return 1;
}

int is_legal_bishop_move(const Board* board, Move move) {
  int start_row = rank_of(move.from);
  int start_col = file_of(move.from);
  int end_row = rank_of(move.to);
  int end_col = file_of(move.to);
  int cols_moved = end_col - start_col;
  int rows_moved = end_row - start_row;
  if (abs(cols_moved) != abs(rows_moved) ||
      check_line(board, start_row, start_col, rows_moved, cols_moved)) {
    return 0;
  }
  return 1;
}

int is_legal_rook_move(const Board* board, Move move) {
  int start_row = rank_of(move.from);
  int start_col = file_of(move.from);
  int end_row = rank_of(move.to);
  int end_col = file_of(move.to);
  int cols_moved = end_col - start_col;
  int rows_moved = end_row - start_row;
  if ((cols_moved != 0 && rows_moved != 0) ||
      check_line(board, start_row, start_col, rows_moved, cols_moved)) {
    return 0;
  }
  return 1;
}

int is_legal_queen_move(const Board* board, Move move) {
  int start_row = rank_of(move.from);
  int start_col = file_of(move.from);
  int end_row = rank_of(move.to);
  int end_col = file_of(move.to);
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
  int start_row = rank_of(move.from);
  int start_col = file_of(move.from);
  int end_row = rank_of(move.to);
  int end_col = file_of(move.to);
  Piece piece = board_get_piece(board, move.from);

  // standard king move
  if ((abs(start_row - end_row) < 2 && abs(start_col - end_col) < 2)) {
    return 1;
  }

  // if king in check then cannot castle
  if (can_be_attacked_by(board, -board->turn, move.from)) {
    return 0;
  }

  // queen side castle
  if (rank_of(move.from) == rank_of(move.to) &&
      !can_be_attacked_by(board, -board->turn, move.from - 2) &&
      !can_be_attacked_by(board, -board->turn, move.from - 1) &&
      board_get_piece(board, move.from - 1).type == EMPTY &&
      board_get_piece(board, move.from - 2).type == EMPTY &&
      board_get_piece(board, move.from - 3).type == EMPTY && start_col == 4 &&
      end_col == 2 &&
      ((piece.colour == WHITE && board->rights & WHITE_QUEEN_SIDE) ||
       (piece.colour == BLACK && board->rights & BLACK_QUEEN_SIDE))) {
    return 1;
  }

  // king side castle
  if (rank_of(move.from) == rank_of(move.to) &&
      !can_be_attacked_by(board, -board->turn, move.from + 1) &&
      !can_be_attacked_by(board, -board->turn, move.from + 2) &&
      board_get_piece(board, move.from + 1).type == EMPTY &&
      board_get_piece(board, move.from + 2).type == EMPTY && start_col == 4 &&
      end_col == 6 &&
      ((piece.colour == WHITE && board->rights & WHITE_KING_SIDE) ||
       (piece.colour == BLACK && board->rights & BLACK_KING_SIDE))) {
    return 1;
  }

  return 0;
}

int is_psuedo_legal_move(const Board* board, Move move) {
  Piece piece = board_get_piece(board, move.from);

  // null move
  if (move.from == move.to) {
    return 0;
  }

  // trying to capture own piece
  if (piece.colour == board_get_piece(board, move.to).colour) {
    return 0;
  }
  // check movement follows rules of chess
  switch (piece.type) {
    case EMPTY:
    case INVALID:
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
  if (move.from < 0 || move.from >= 64 || move.to < 0 || move.to >= 64) {
    return 0;
  }

  // trying to move piece whose turn it isn't
  if (board_get_piece(board, move.from).colour != board->turn) {
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
