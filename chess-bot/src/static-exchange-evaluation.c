#include "../../chess-lib/include/chess-lib.h"
#include "../include/bot-types.h"

/*

typedef struct {
  sq0x88_t attacker_list[16];
  sq0x88_t defender_list[16];
  size_t attacker_count, defender_count;
} attacker_list_t;

// finds all direct attackers and defenders
void generate_attackers(attacker_list_t* attackers, sq0x88_t from, sq0x88_t to) {
  size_t attacker_count = 0;
  piece_list_t* attacker_pl;
  piece_list_t* defender_pl;
  sq0x88_t attacker_pawn_inc;
  sq0x88_t defender_pawn_inc;
  piece_t attacker_colour;
  piece_t defender_colour;

  sq0x88_t first_attack_increment = queen_increment(from, to);

  if (board.black_to_move) {
    attacker_pl = &black_piece_list;
    defender_pl = &white_piece_list;
    attacker_pawn_inc = black_pawn_push_inc;
    defender_pawn_inc = white_pawn_push_inc;
    attacker_colour = BLACK;
    defender_colour = WHITE;
  } else {
    attacker_pl = &white_piece_list;
    defender_pl = &black_piece_list;
    attacker_pawn_inc = white_pawn_push_inc;
    defender_pawn_inc = black_pawn_push_inc;
    attacker_colour = WHITE;
    defender_colour = BLACK;
  }

  // pawns
  sq0x88_t pawn_square;
  pawn_square = to - attacker_pawn_inc + 1;
  if (pawn_square != from && piece(pawn_square) == (PAWN | attacker_colour)) {
    attackers->attacker_list[attackers->attacker_count++] = pawn_square;
  }
  pawn_square = to - attacker_pawn_inc + 1;
  if (pawn_square != from && piece(pawn_square) == (PAWN | attacker_colour)) {
    attackers->attacker_list[attackers->attacker_count++] = pawn_square;
  }
  pawn_square = to - defender_pawn_inc + 1;
  if (piece(pawn_square) == (PAWN | defender_colour)) {
    attackers->defender_list[attackers->defender_count++] = pawn_square;
  }
  pawn_square = to - defender_pawn_inc + 1;
  if (piece(pawn_square) == (PAWN | defender_colour)) {
    attackers->defender_list[attackers->defender_count++] = pawn_square;
  }
  // knights
  for (int i = 0; i < attacker_pl->knight_count; i++) {
    sq0x88_t knight_square = attacker_pl->knight_list[i];
    if (knight_square != from && !knight_increment(knight_square, to)) continue;
    attackers->attacker_list[attackers->attacker_count++] = knight_square;
  }
  for (int i = 0; i < defender_pl->knight_count; i++) {
    sq0x88_t knight_square = defender_pl->knight_list[i];
    if (!knight_increment(knight_square, to)) continue;
    attackers->defender_list[attackers->defender_count++] = knight_square;
  }
  // bishops
  for (int i = 0; i < bishop_increments_count; i++) {
    sq0x88_t inc = bishop_increments_list[i];
    sq0x88_t square;
    if (first_attack_increment == inc) {
      square = forwards_ray_cast(from, inc);
    } else {
      square = forwards_ray_cast(to, inc);
    }
    piece_t p = piece(square);
    if (!(p & BISHOP)) continue;
    if (p & attacker_colour) {
      attackers->attacker_list[attackers->attacker_count++] = square;
    } else {
      attackers->defender_list[attackers->defender_count++] = square;
    }
  }
  // rooks
  for (int i = 0; i < rook_increments_count; i++) {
    sq0x88_t inc = rook_increments_list[i];
    sq0x88_t square;
    if (first_attack_increment == inc) {
      square = forwards_ray_cast(from, inc);
    } else {
      square = forwards_ray_cast(to, inc);
    }
    piece_t p = piece(square);
    if (!(p & ROOK)) continue;
    if (p & attacker_colour) {
      attackers->attacker_list[attackers->attacker_count++] = square;
    } else {
      attackers->defender_list[attackers->defender_count++] = square;
    }
  }

  // king
  sq0x88_t king_square;
  king_square = attacker_pl->king_square;
  if (king_increment(king_square, to)) {
    attackers->attacker_list[attackers->attacker_count++] = king_square;
  }
  king_square = defender_pl->king_square;
  if (king_increment(king_square, to)) {
    attackers->defender_list[attackers->defender_count++] = king_square;
  }
}

void update_xray_attacker(attacker_list_t* attackers, sq0x88_t from, sq0x88_t to) {
  piece_t attacker_colour;
  piece_t defender_colour;
  if (board.black_to_move) {
    attacker_colour = BLACK;
    defender_colour = WHITE;
  } else {
    attacker_colour = WHITE;
    defender_colour = BLACK;
  }
  sq0x88_t inc = queen_increment(from, to);
  sq0x88_t square = forwards_ray_cast(from, inc);
  if (off_the_board(square)) return;
  piece_t attacker = piece(square);
  if (!(bishop_increment(from, to) && attacker & BISHOP) &&
      !(rook_increment(from, to) && attacker & ROOK))
    return;
  if (attacker & attacker_colour) {
    attackers->attacker_list[attackers->attacker_count++] = square;
  } else {
    attackers->defender_list[attackers->defender_count++] = square;
  }
}

sq0x88_t next_attacker(attacker_list_t* attackers, size_t start) {
    size_t least_valuable_attacker = start;
    score_centipawn_t least_valuable_attacker_value = board_value(attackers[least_valuable_attacker]);
    // attacker list is sorted up until sliding pieces
    if (least_valuable_attacker_value <= 3) return attackers->attacker_list[least_valuable_attacker];
    for (size_t i = start + 1; i < attackers->attacker_count; i++) {
        if (board_value(attackers[i]) < least_valuable_attacker_value) {
            least_valuable_attacker = i;
            least_valuable_attacker_value = board_value(attackers[i]);
        }
    }
    sq0x88_t  least_valuable_attacker_square = attackers->attacker_list[least_valuable_attacker];
    attackers->attacker_list[least_valuable_attacker] = attackers->attacker_list[start];
    attackers->attacker_list[start] = least_valuable_attacker_square;
    return least_valuable_attacker_square;
}

score_centipawn_t static_exchange_evaluation(sq0x88_t from, sq0x88_t to) {
  score_centipawn_t alpha, beta;
  score_centipawn_t value, value_attacker;
  sq0x88_t next;

  value = board_value(to);
  alpha = MIN_SCORE;
  beta = value;
  next = from;

  generate_attackers(next, to);

  if (black_attackers() == 0) return value;
  if (piece(from) == WHITE_KING) return alpha;
  value_attacker = board_value(next);

  for (;;) {
    value -= value_attacker;
    if (value >= beta) return beta;
    if (value > alpha) alpha = value;
    if (alpha > 0) return alpha;
    next = next_black_attacker();
    update_xray_attackers(next, to);
    if (white_attackers() == 0) break;
    if (piece(next) == BLACK_KING) return beta;
    value_attacker = board_value(next);

    value += value_attacker;
    if (value <= alpha) return alpha;
    if (value < alpha) beta = value;
    if (beta < 0) return beta;
    next = next_white_attacker();
    update_xray_attackers(next, to);
    if (black_attackers() == 0) break;
    if (piece(next) == WHITE_KING) return alpha;
    value_attacker = board_value(next);
  }
  if (value < alpha) return alpha;
  if (value > beta) return beta;
  return value;
}

attacker_t attacker(sq0x88_t square, piece_t piece) {
  return (attacker_t){square, piece};
}

// finds all direct attackers and defenders of target square with the colour
// "attacker_colour". returns the list of attackers sorted in order of least
// valuable to most
size_t find_attackers(attacker_t* attacker_list, sq0x88_t target_square,
                      piece_t attacker_colour) {
  size_t attacker_count = 0;
  piece_list_t* pl;
  sq0x88_t pawn_inc;
  if (attacker_colour == WHITE) {
    pl = &white_piece_list;
    pawn_inc = white_pawn_push_inc;
  } else {
    pl = &black_piece_list;
    pawn_inc = black_pawn_push_inc;
  }

  // pawns
  sq0x88_t pawn_square;
  pawn_square = target_square - pawn_inc + 1;
  if (piece(pawn_square) == (PAWN | attacker_colour)) {
    attacker_list[attacker_count++] =
        attacker(pawn_square, (PAWN | attacker_colour));
  }
  pawn_square = target_square - pawn_inc - 1;
  if (piece(pawn_square) == (PAWN | attacker_colour)) {
    attacker_list[attacker_count++] =
        attacker(pawn_square, (PAWN | attacker_colour));
  }
  // knights
  for (int i = 0; i < pl->knight_count; i++) {
    sq0x88_t knight_square = pl->knight_list[i];
    if (!knight_increment(knight_square, target_square)) continue;
    attacker_list[attacker_count++] =
        attacker(knight_square, (KNIGHT | attacker_colour));
  }
  size_t non_sliding_attacker_count = attacker_count;
  for (int i = 0; i < bishop_increments_count; i++) {
    sq0x88_t inc = bishop_increments_list[i];
    sq0x88_t attacker_square = target_square;
    do {
      attacker_square = forwards_ray_cast(attacker_square, inc);
      piece_t attacker_piece = piece(attacker_square);
      if (attacker_piece & BISHOP && attacker_piece & attacker_colour) {
        attacker_list[attacker_count++] =
            attacker(attacker_square, attacker_piece);
        continue;
      }
    } while (0);
  }

  for (int i = 0; i < rook_increments_count; i++) {
    sq0x88_t inc = rook_increments_list[i];
    sq0x88_t attacker_square = target_square;
    do {
      attacker_square = forwards_ray_cast(attacker_square, inc);
      piece_t attacker_piece = piece(attacker_square);
      if (attacker_piece & ROOK && attacker_piece & attacker_colour) {
        attacker_list[attacker_count++] =
            attacker(attacker_square, attacker_piece);
        continue;
      }
    } while (0);
  }

  for (size_t i = non_sliding_attacker_count; i < attacker_count; i++) {
    for (size_t j = i; j < attacker_count - 1; j++) {
      if (value_diff(attacker_list[j].piece, attacker_list[j + 1].piece) > 0 &&
          (queen_increment(attacker_list[j + 0].square, target_square) !=
           queen_increment(attacker_list[j + 1].square, target_square))) {
        attacker_t t = attacker_list[j + 0];
        attacker_list[j + 0] = attacker_list[j + 1];
        attacker_list[j + 1] = t;
      }
    }
  }

  // king
  sq0x88_t king_square = pl->king_square;
  if (king_increment(king_square, target_square)) {
    attacker_list[attacker_count++] =
        attacker(king_square, (KING | attacker_colour));
  }

  return attacker_count;
}

*/