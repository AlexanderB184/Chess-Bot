#include "move.h"

#include "board.h"

Piece board_get_piece(const Board* board, int square) {
  if (square < 0 || square >= 64) {
    return (Piece){NONE, INVALID};
  }
  return board->pieces[square];
}

void board_set_piece(Board* board, int square, Piece piece) {
  if (square < 0 || square >= 64) {
    return;
  }
  board->pieces[square] = piece;
}

void board_move_piece(Board* board, int from, int to) {
  board_set_piece(board, to, board_get_piece(board, from));
  board_set_piece(board, from, (Piece){NONE, EMPTY});
}

void make_move(Board* board, Move move) {
  Piece piece = board_get_piece(board, move.from);

  if (piece.type == PAWN) {
    board->half_move_count = 0;
  } else {
    board->half_move_count++;
  }

  // castling
  if (piece.type == KING && file_of(move.from) == 4 && file_of(move.to) == 2) {
    board_move_piece(board, move.from - 4, move.from - 1);
  }
  if (piece.type == KING && file_of(move.from) == 4 && file_of(move.to) == 6) {
    board_move_piece(board, move.from + 3, move.from + 1);
  }

  // enpassent
  if (piece.type == PAWN && file_of(move.to) == file_of(board->enpassent) &&
      rank_of(move.from) == rank_of(board->enpassent)) {
    board_set_piece(board, board->enpassent, (Piece){NONE, EMPTY});
  }

  // double pawn move
  if (piece.type == PAWN && abs(rank_of(move.to) - rank_of(move.from)) == 2) {
    board->enpassent = move.to;
  } else {
    board->enpassent = -1;
  }
  board_move_piece(board, move.from, move.to);

  // promotion
  if (piece.type == PAWN && (rank_of(move.to) == 0 || rank_of(move.to) == 7)) {
    board_set_piece(board, move.to, (Piece){board->turn, move.promote_to});
  }

  // castle rights
  if (move.from == 0 || move.to == 0) {
    board->rights &= ~WHITE_QUEEN_SIDE;
  }
  if (move.from == 7 || move.to == 7) {
    board->rights &= ~WHITE_KING_SIDE;
  }
  if (move.from == 56 || move.to == 56) {
    board->rights &= ~BLACK_QUEEN_SIDE;
  }
  if (move.from == 63 || move.to == 63) {
    board->rights &= ~BLACK_KING_SIDE;
  }
  if (move.from == 4 || move.to == 4) {
    board->rights &= ~WHITE_BOTH_SIDES;
  }
  if (move.from == 60 || move.to == 60) {
    board->rights &= ~BLACK_BOTH_SIDES;
  }

  board->turn = -board->turn;
  board->move_count++;
}

Board copy_make_move(const Board* board, Move move) {
  Board cloned_board = *board;
  make_move(&cloned_board, move);
  return cloned_board;
}
