#include <stdlib.h>
#include <stdio.h>

#include "../include/chess-lib.h"
#include "../include/private/chess-lib-internals.h"
int is_legal_enpassent(const chess_state_t* chess_state, move_t move) {
  sq0x88_t from = get_from(move);
  sq0x88_t king_square = chess_state->friendly_pieces->king_square;
  sq0x88_t inc;

  inc = bishop_increment(king_square, enpassent_target(chess_state));

  if (inc) {
    if (forwards_ray_cast(chess_state, king_square, inc) !=
        enpassent_target(chess_state))
      return 1;

    sq0x88_t pinning_square =
        forwards_ray_cast(chess_state, enpassent_target(chess_state), inc);

    if (off_the_board(pinning_square) ||
        piece_is_friendly(chess_state, pinning_square))
      return 1;

    if (piece(chess_state, pinning_square) & BISHOP) return 0;

  } else {
    inc = rook_increment(king_square, enpassent_target(chess_state));

    if (inc == 0) return 1;

    if (inc != rook_increment(king_square, from)) return 1;

    sq0x88_t square = forwards_ray_cast(chess_state, king_square, inc);

    if (square != enpassent_target(chess_state) && square != from) return 1;

    square = forwards_ray_cast(chess_state, square + inc, inc);

    if (off_the_board(square) || piece_is_friendly(chess_state, square))
      return 1;

    if (piece(chess_state, square) & ROOK) return 0;
  }

  return 1;
}

void trace_ply_stack(const chess_state_t* chess_state) {
  for (int i = 0; i < chess_state->ply_counter; i++) {
    printf("make_move(move(%d, %d, %d));\n",
           get_from(chess_state->ply_stack[i].move),
           get_to(chess_state->ply_stack[i].move),
           get_flags(chess_state->ply_stack[i].move));
  }
}

// checks if pseudo legal move is legal, assumes position is not in check
int is_legal(const chess_state_t* chess_state, move_t move) {
  sq0x88_t from = get_from(move);
  sq0x88_t to = get_to(move);

  if (piece(chess_state, to) & KING) {
    trace_ply_stack(chess_state);
    printf("can capture king\n");
    exit(1);
  }
  sq0x88_t inc;
  sq0x88_t king_square = chess_state->friendly_pieces->king_square;

  if (from == king_square) {  // king moves
    if (under_attack(chess_state, to, chess_state->enemy_colour)) return 0;
    if (get_flags(move) == QUEEN_CASTLE) {
      if (under_attack(chess_state, from - 1, chess_state->enemy_colour))
        return 0;
    }
    if (get_flags(move) == KING_CASTLE) {
      if (under_attack(chess_state, from + 1, chess_state->enemy_colour))
        return 0;
    }
    return 1;
  }
  if (get_flags(move) == ENPASSENT) {
    if (!is_legal_enpassent(chess_state, move)) return 0;
  }
  // checking moved piece is not pinned
  inc = queen_increment(king_square, from);
  if (inc == 0) return 1;
  if (inc == queen_increment(king_square, to)) return 1;
  if (forwards_ray_cast(chess_state, king_square, inc) != from) return 1;
  sq0x88_t pinning_square = forwards_ray_cast(chess_state, from, inc);
  if (off_the_board(pinning_square) ||
      piece_is_friendly(chess_state, pinning_square))
    return 1;
  // only sliding pieces can pin
  if (((piece(chess_state, pinning_square) & BISHOP) &&
       bishop_increment(king_square, pinning_square)) ||
      ((piece(chess_state, pinning_square) & ROOK) &&
       rook_increment(king_square, pinning_square))) {
    return 0;
  }
  return 1;
}