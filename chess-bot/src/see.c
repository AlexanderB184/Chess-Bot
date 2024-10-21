#include "../include/static-exchange-evaluation.h"

#include "../../chess-lib/include/chess-lib.h"
#include "../../chess-lib/include/private/chess-lib-internals.h"
#include "../include/bot.h"
#include "../include/static-evaluation.h"

void add_attacker(attacker_list_t* attackers, sq0x88_t square,
                  score_cp_t piece_value) {
  attackers->pieces[attackers->piece_count++] =
      (attacker_t){square, piece_value};
}

// finds all direct attackers and defenders
void generate_attackers(const chess_state_t* chess_state,
                        attacker_list_t* attackers, attacker_list_t* defenders,
                        sq0x88_t from, sq0x88_t to) {
  *attackers = (attacker_list_t){.piece_count = 0};
  *defenders = (attacker_list_t){.piece_count = 0};

  sq0x88_t first_attack_increment = queen_increment(from, to);

  // pawns
  sq0x88_t pawn_square;
  pawn_square = to - chess_state->up_increment + 1;
  if (pawn_square != from && piece(chess_state, pawn_square) ==
                                 (PAWN | chess_state->friendly_colour)) {
    add_attacker(attackers, pawn_square,
                 pawn_value(pawn_square, chess_state->friendly_colour));
  }
  pawn_square = to - chess_state->up_increment + 1;
  if (pawn_square != from && piece(chess_state, pawn_square) ==
                                 (PAWN | chess_state->friendly_colour)) {
    add_attacker(attackers, pawn_square,
                 pawn_value(pawn_square, chess_state->friendly_colour));
  }
  pawn_square = to - chess_state->down_increment + 1;
  if (piece(chess_state, pawn_square) == (PAWN | chess_state->enemy_colour)) {
    add_attacker(attackers, pawn_square,
                 pawn_value(pawn_square, chess_state->enemy_colour));
  }
  pawn_square = to - chess_state->down_increment + 1;
  if (piece(chess_state, pawn_square) == (PAWN | chess_state->enemy_colour)) {
    add_attacker(attackers, pawn_square,
                 pawn_value(pawn_square, chess_state->enemy_colour));
  }
  // knights
  for (int i = 0; i < chess_state->friendly_pieces->knight_count; i++) {
    sq0x88_t knight_square = chess_state->friendly_pieces->knight_list[i];
    if (knight_square != from && !knight_increment(knight_square, to)) continue;
    add_attacker(attackers, knight_square,
                 knight_value(pawn_square, chess_state->friendly_colour));
  }
  for (int i = 0; i < chess_state->enemy_pieces->knight_count; i++) {
    sq0x88_t knight_square = chess_state->enemy_pieces->knight_list[i];
    if (!knight_increment(knight_square, to)) continue;
    add_attacker(defenders, knight_square,
                 knight_value(pawn_square, chess_state->enemy_colour));
  }
  // bishops
  for (int i = 0; i < bishop_increments_count; i++) {
    sq0x88_t inc = bishop_increments_list[i];
    sq0x88_t square;
    if (first_attack_increment == inc) {
      square = forwards_ray_cast(chess_state, from, inc);
    } else {
      square = forwards_ray_cast(chess_state, to, inc);
    }
    piece_t p = piece(chess_state, square);

    if (!(p & BISHOP)) continue;

    if (p & chess_state->friendly_colour) {
      add_attacker(attackers, square, piece_value(square, p));

    } else {
      add_attacker(attackers, square, piece_value(square, p));
    }
  }
  // rooks
  for (int i = 0; i < rook_increments_count; i++) {
    sq0x88_t inc = rook_increments_list[i];
    sq0x88_t square;
    if (first_attack_increment == inc) {
      square = forwards_ray_cast(chess_state, from, inc);
    } else {
      square = forwards_ray_cast(chess_state, to, inc);
    }
    piece_t p = piece(chess_state, square);
    if (!(p & ROOK)) continue;

    if (p & chess_state->friendly_colour) {
      add_attacker(attackers, square, piece_value(square, p));
    } else {
      add_attacker(defenders, square, piece_value(square, p));
    }
  }

  // king
  sq0x88_t king_square;
  king_square = chess_state->friendly_pieces->king_square;
  if (king_increment(king_square, to)) {
    add_attacker(attackers, king_square,
                 king_value(king_square, chess_state->friendly_colour));
  }
  king_square = chess_state->enemy_pieces->king_square;
  if (king_increment(king_square, to)) {
    add_attacker(defenders, king_square,
                 king_value(king_square, chess_state->enemy_colour));
  }
}

void update_xray_attackers(const chess_state_t* chess_state,
                          attacker_list_t* attackers,
                          attacker_list_t* defenders, sq0x88_t from,
                          sq0x88_t to) {
  sq0x88_t inc = queen_increment(from, to);
  if (!inc) return;
  sq0x88_t square = forwards_ray_cast(chess_state, from, inc);
  if (off_the_board(square)) return;
  piece_t attacker = piece(chess_state, square);
  if (!(bishop_increment(from, to) && attacker & BISHOP) &&
      !(rook_increment(from, to) && attacker & ROOK))
    return;
  score_cp_t value = piece_value(square, attacker);
  if (attacker & chess_state->friendly_colour) {
    add_attacker(attackers, square, value);
  } else {
    add_attacker(defenders, square, value);
  }
}

// select and erase the least valuable attacker, returning the square
attacker_t next_attacker(attacker_list_t* attackers) {
  score_cp_t least_value = attackers->pieces[0].value;
  int least_index = 0;
  for (int i = 1; i < attackers->piece_count; i++) {
    if (attackers->pieces[i].value < least_value) {
      least_value = attackers->pieces[i].value;
      least_index = i;
    }
  }
  attacker_t attacker = attackers->pieces[least_index];

  attackers->pieces[least_index] = attackers->pieces[--attackers->piece_count];

  return attacker;
}

score_cp_t static_exchange_evaluation(const chess_state_t* chess_state,
                                             move_t move) {
  score_cp_t alpha, beta;
  score_cp_t value, value_attacker;
  attacker_t next;
  sq0x88_t from = get_from(move);
  sq0x88_t to = get_to(move);

  value = piece_value(to, piece(chess_state, to));
  alpha = MIN_SCORE;
  beta = value;
  next.square = from;
  next.value = piece_value(from, piece(chess_state, from));

  attacker_list_t attacker_list;
  attacker_list_t defender_list;

  generate_attackers(chess_state, &attacker_list, &defender_list, from, to);

  if (attacker_list.piece_count == 0) return value;
  if (piece(chess_state, from) & KING) return alpha;
  value_attacker = next.value;
  for (;;) {

    value -= value_attacker;
    if (value >= beta) return beta;
    if (value > alpha) alpha = value;
    if (alpha > 0) return alpha;
    next = next_attacker(&defender_list);
    update_xray_attackers(chess_state, &attacker_list, &defender_list,
                          next.square, to);

    if (attacker_list.piece_count <= 0) break;
    if (piece(chess_state, next.square) & KING) return beta;
    value_attacker = next.value;

    value += value_attacker;
    if (value <= alpha) return alpha;
    if (value < alpha) beta = value;
    if (beta < 0) return beta;
    next = next_attacker(&attacker_list);
    update_xray_attackers(chess_state, &attacker_list, &defender_list,
                          next.square, to);
    if (defender_list.piece_count <= 0) break;
    if (piece(chess_state, next.square) & KING) return alpha;
    value_attacker = next.value;
  }
  if (value < alpha) return alpha;
  if (value > beta) return beta;
  return value;
}