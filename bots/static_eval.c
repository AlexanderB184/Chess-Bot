#include "static_eval.h"

double pawn_square_table[64] = {
    0.00,  0.00,  0.00,  0.00, 0.00,  0.00,  0.00, 0.00,  0.50,  0.50,  0.50,
    0.50,  0.50,  0.50,  0.50, 0.50,  0.10,  0.10, 0.20,  0.30,  0.30,  0.20,
    0.10,  0.10,  0.05,  0.05, 0.10,  0.25,  0.25, 0.10,  0.05,  0.05,  0.00,
    -0.05, -0.05, 0.20,  0.20, -0.05, -0.05, 0.00, 0.05,  -0.05, -0.10, 0.00,
    0.00,  -0.10, -0.05, 0.05, 0.05,  0.10,  0.10, -0.20, -0.20, 0.10,  0.10,
    0.05,  0.00,  0.00,  0.00, 0.00,  0.00,  0.00, 0.00,  0.00,
};

double knight_square_table[64] = {
    -0.50, -0.40, -0.30, -0.30, -0.30, -0.30, -0.40, -0.50, -0.40, -0.20, 0.00,
    0.00,  0.00,  0.00,  -0.20, -0.40, -0.30, 0.00,  0.10,  0.15,  0.15,  0.10,
    0.00,  -0.30, -0.30, 0.05,  0.15,  0.25,  0.25,  0.15,  0.05,  -0.30, -0.30,
    0.00,  0.15,  0.25,  0.25,  0.15,  0.00,  -0.30, -0.30, 0.05,  0.10,  0.15,
    0.15,  0.10,  0.05,  -0.30, -0.40, -0.20, 0.00,  0.05,  0.05,  0.00,  -0.20,
    -0.40, -0.50, -0.40, -0.30, -0.30, -0.30, -0.30, -0.40, -0.50,
};

double bishop_square_table[64] = {
    -0.20, -0.10, -0.10, -0.10, -0.10, -0.10, -0.10, -0.20, -0.10, 0.00,  0.00,
    0.00,  0.00,  0.00,  0.00,  -0.10, -0.10, 0.00,  0.05,  0.10,  0.10,  0.05,
    0.00,  -0.10, -0.10, 0.05,  0.05,  0.10,  0.10,  0.05,  0.05,  -0.10, -0.10,
    0.00,  0.10,  0.10,  0.10,  0.10,  0.00,  -0.10, -0.10, 0.10,  0.10,  0.10,
    0.10,  0.10,  0.10,  -0.10, -0.10, 0.05,  0.00,  0.00,  0.00,  0.00,  0.05,
    -0.10, -0.20, -0.10, -0.10, -0.10, -0.10, -0.10, -0.10, -0.20,
};

double rook_square_table[64] = {
    0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.05, 0.10,  0.10,
    0.10,  0.10,  0.10,  0.10,  -0.05, -0.05, 0.00,  0.00,  0.00, 0.00,  0.00,
    0.00,  -0.05, -0.05, 0.00,  0.00,  0.00,  0.00,  0.00,  0.00, -0.05, -0.05,
    0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  -0.05, -0.05, 0.00, 0.00,  0.00,
    0.00,  0.00,  0.00,  -0.05, -0.05, 0.00,  0.00,  0.00,  0.00, 0.00,  0.00,
    -0.05, 0.00,  0.00,  0.00,  0.05,  0.05,  0.00,  0.00,  0.00,
};

double queen_square_table[64] = {
    -0.20, -0.10, -0.10, -0.05, -0.05, -0.10, -0.10, -0.20, -0.10, 0.00,  0.00,
    0.00,  0.00,  0.00,  0.00,  -0.10, -0.10, 0.00,  0.05,  0.05,  0.05,  0.05,
    0.00,  -0.10, -0.05, 0.00,  0.05,  0.05,  0.05,  0.05,  0.00,  -0.05, 0.00,
    0.00,  0.05,  0.05,  0.05,  0.05,  0.00,  -0.05, -0.05, 0.05,  0.05,  0.05,
    0.05,  0.05,  0.00,  -0.10, -0.05, 0.00,  0.05,  0.00,  0.00,  0.00,  0.00,
    -0.10, -0.20, -0.10, -0.10, -0.05, -0.05, -0.10, -0.10, -0.20,
};

double king_square_table[64] = {
    -0.30, -0.40, -0.50, -0.50, -0.50, -0.40, -0.40, -0.30, -0.30, -0.40, -0.50,
    -0.50, -0.50, -0.40, -0.40, -0.30, -0.30, -0.40, -0.50, -0.50, -0.50, -0.40,
    -0.40, -0.30, -0.30, -0.40, -0.50, -0.50, -0.50, -0.40, -0.40, -0.30, -0.20,
    -0.30, -0.30, -0.40, -0.40, -0.30, -0.30, -0.20, -0.10, -0.20, -0.20, -0.20,
    -0.20, -0.20, -0.20, -0.10, 0.20,  0.20,  0.00,  0.00,  0.00,  0.00,  0.20,
    0.20,  0.20,  0.30,  0.10,  0.00,  0.00,  0.10,  0.30,  0.20,
};

double king_endgame_square_table[64] = {
    -0.50, -0.40, -0.30, -0.20, -0.20, -0.30, -0.40, -0.50, -0.30, -0.20, -0.10,
    0.00,  0.00,  -0.10, -0.20, -0.30, -0.30, -0.10, 0.20,  0.30,  0.30,  0.20,
    -0.10, -0.30, -0.30, -0.10, 0.30,  0.40,  0.40,  0.30,  -0.10, -0.30, -0.30,
    -0.10, 0.30,  0.40,  0.40,  0.30,  -0.10, -0.30, -0.30, -0.10, 0.20,  0.30,
    0.30,  0.20,  -0.10, -0.30, -0.30, -0.30, 0.00,  0.00,  0.00,  0.00,  -0.30,
    -0.30, -0.50, -0.40, -0.30, -0.20, -0.20, -0.30, -0.40, -0.50,
};

double piece_square_value(Piece, int, int);

double static_evaluation(const Board* board) {
  if (is_stalemate(board)) {
    if (is_check(board)) {
      return -200.0f;
    } else {
      return 0.0f;
    }
  }
  int endgame = is_endgame(board);
  double material_value = 0.0f;
  for (int square = 0; square < 64; square++) {
    Piece piece = board_get_piece(board, square);
    if (piece.type == EMPTY) continue;
    material_value +=
        piece.colour * (static_piece_value(piece.type) +
                        piece_square_value(piece, square, endgame));
  }
  return material_value * board->turn;
}

double piece_square_value(Piece piece, int square, int endgame) {
  int effective_square = (piece.colour == WHITE)
                             ? ((7 - rank_of(square)) * 8 + file_of(square))
                             : (square);
  switch (piece.type) {
    case KING:
      if (endgame) {
        return king_endgame_square_table[effective_square];
      } else {
        return king_square_table[effective_square];
      }
    case QUEEN:
      return queen_square_table[effective_square];
    case ROOK:
      return rook_square_table[effective_square];
    case BISHOP:
      return bishop_square_table[effective_square];
    case KNIGHT:
      return knight_square_table[effective_square];
    case PAWN:
      return pawn_square_table[effective_square];
    default:
      return 0.0;
  }
}

double static_piece_value(Piece_Type type) {
  switch (type) {
    case KING:
      return 200.0;
    case QUEEN:
      return 9.0;
    case ROOK:
      return 5.0;
    case BISHOP:
      return 3.3;
    case KNIGHT:
      return 3.2;
    case PAWN:
      return 1.0;
    default:
      return 0.0;
  }
}

int is_endgame(const Board* board) {
  int wqueen = 0, bqueen = 0, wminor = 0, bminor = 0, wmajor = 0, bmajor = 0;
  for (int square = 0; square < 64; square++) {
    Piece piece = board_get_piece(board, square);
    if (piece.type == QUEEN) {
      if (piece.colour == WHITE) {
        wqueen++;
      } else {
        bqueen++;
      }
      if (wqueen && bqueen) {
        return 0;
      }
    }
    if (piece.type == ROOK) {
      if (piece.colour == WHITE) {
        wmajor++;
      } else {
        bmajor++;
      }
    }
    if (piece.type == KNIGHT || piece.type == BISHOP) {
      if (piece.colour == WHITE) {
        wminor++;
      } else {
        bminor++;
      }
    }
  }
  return (!wqueen || (wmajor < 0 && wminor <= 1)) &&
         (!bqueen || (bmajor < 0 && bminor <= 1));
}

int is_promotion(const Board* board, Move move) {
  return board_get_piece(board, move.from).type == PAWN &&
         (rank_of(move.to) == 0 || rank_of(move.to) == 7);
}

int is_pawn_promoting(const Board* board) {
  int rank_to_check = (board->turn == WHITE) ? (48) : (8);
  for (int file = 0; file < 8; file++) {
    Piece p = board->pieces[rank_to_check + file];
    if (p.type == PAWN && p.colour == board->turn) {
      return 1;
    }
  }
  return 0;
}

int is_capture(const Board* board, Move move) {
  Piece piece_to_move = board_get_piece(board, move.from);
  Piece piece_to_capture = board_get_piece(board, move.to);
  // is capture
  return piece_to_capture.type != EMPTY ||
         (piece_to_move.type == PAWN &&
          rank_of(move.from) == rank_of(board->enpassent) &&
          file_of(move.to) == file_of(board->enpassent));
}
