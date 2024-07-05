#include "../include/chess-lib.h"
#include "../include/private/chess-lib-internals.h"

enum piece_values {
  PAWN_VALUE = 1,
  BISHOP_VALUE = 3,
  KNIGHT_VALUE = 3,
  ROOK_VALUE = 5,
  QUEEN_VALUE = 9,
  KING_VALUE = 200,
};

enum move_priority {
  MOVE_PRIORITY_PV = 3 * KING_VALUE,
  MOVE_PRIORITY_HASH_MOVE = 2 * KING_VALUE,
  MOVE_PRIORITY_WINNING_CAPTURE,  // priority is between KING_VALUE + 1 and 2 *
                                  // KING_VALUE - 1 inclusive
  MOVE_PRIORITY_NEUTRAL_CAPTURE = KING_VALUE,  // priority = KING_VALUE exactly
  MOVE_PRIORITY_LOSING_CAPTURE,  // priority is between 1 and KING_VALUE - 1
                                 // inclusive
  MOVE_PRIORITY_QUIET_MOVE = 0
};

int value_of(piece_t piece) {
  switch (piece & PIECE_MASK) {
    case PAWN:
      return PAWN_VALUE;
    case KNIGHT:
      return KNIGHT_VALUE;
    case BISHOP:
      return BISHOP_VALUE;
    case ROOK:
      return ROOK_VALUE;
    case QUEEN:
      return QUEEN_VALUE;
    case KING:
      return KING_VALUE;
    default:
      return 0;
  }
}

int value_of_promotion(move_flags_t piece) {
  switch (piece & 0b11) {
    case KNIGHT_PROMOTE_TO:
      return KNIGHT_VALUE;
    case BISHOP_PROMOTE_TO:
      return BISHOP_VALUE;
    case ROOK_PROMOTE_TO:
      return ROOK_VALUE;
    case QUEEN_PROMOTE_TO:
      return QUEEN_VALUE;
    default:
      return 0;
  }
}

#define PRIORITY_MAX 0x0FFF

move_t move(sq0x88_t from, sq0x88_t to, uint16_t flags, piece_t moved_piece, piece_t captured_piece) {
  // TODO: calculate move order priority
  uint16_t priority = MOVE_PRIORITY_QUIET_MOVE;
  if (flags & CAPTURE) {
    int value_delta = value_of(captured_piece) - value_of(moved_piece);
    priority = value_delta + KING_VALUE;
  }
  if (flags & PROMOTION) {
    priority = value_of_promotion(flags) - PAWN_VALUE + KING_VALUE;
  }
  uint16_t bitpacked = (flags & 0xF) | (priority << 4);
  return (move_t){from, to, bitpacked};
}

move_t unordered_move(sq0x88_t from, sq0x88_t to, uint16_t flags) {
  return (move_t){from, to, flags & 0xF};
}

  

