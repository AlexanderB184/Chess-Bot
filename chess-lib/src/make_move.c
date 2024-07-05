// #include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/chess-lib.h"
#include "../include/private/chess-lib-internals.h"

void push_ply_stack(chess_state_t* chess_state, move_t move) {
  if (chess_state->ply_counter >= chess_state->ply_stack_capacity) {
    printf("%d %d\n",chess_state->ply_counter,chess_state->ply_stack_capacity);
    chess_state->ply_stack_capacity = chess_state->ply_stack_capacity * 2;
    chess_state->ply_stack =
        realloc(chess_state->ply_stack,
                sizeof(ply_stack_item_t) * chess_state->ply_stack_capacity);
  }
  chess_state->ply_stack[chess_state->ply_counter].move = move;
  chess_state->ply_stack[chess_state->ply_counter].enpassent =
      chess_state->enpassent_target;
  chess_state->ply_stack[chess_state->ply_counter].half_move_clock =
      chess_state->half_move_clock;
  chess_state->ply_stack[chess_state->ply_counter].rights =
      chess_state->castle_rights;
  chess_state->ply_stack[chess_state->ply_counter].captured =
      piece(chess_state, get_to(move));
  chess_state->ply_stack[chess_state->ply_counter].zobrist =
      chess_state->zobrist;
  chess_state->ply_stack[chess_state->ply_counter].check_square =
      chess_state->check_square;
  chess_state->ply_stack[chess_state->ply_counter].n_checks =
      chess_state->n_checks;
  chess_state->ply_stack[chess_state->ply_counter].discovered_check =
      chess_state->discovered_check;
  chess_state->ply_stack[chess_state->ply_counter].last_irreversible =
      chess_state->ply_of_last_irreversible_move;
  chess_state->ply_counter++;
}

void update_board(chess_state_t* chess_state, move_t move) {
  sq0x88_t from = get_from(move);
  sq0x88_t to = get_to(move);

  if (is_enpassent(move)) {
    remove_piece(chess_state, chess_state->enpassent_target);
  } else if (is_capture(move)) {
    remove_piece(chess_state, to);
  }

  // printf("%d %d %d %d\n", from, to, piece(from), board.black_to_move);
  //  move piece
  move_piece(chess_state, from, to);

  if (is_promotion(move)) {
    remove_piece(chess_state, to);
    place_piece(chess_state, to,
                get_promotes_to(move) | chess_state->friendly_colour);
  }

  if (is_king_castle(move)) {
    move_piece(chess_state, from + (sq0x88_t)3, to - (sq0x88_t)1);
  }
  if (is_queen_castle(move)) {
    move_piece(chess_state, from - (sq0x88_t)4, to + (sq0x88_t)1);
  }
}

void update_rights(chess_state_t* chess_state, move_t move) {
  sq0x88_t from = get_from(move);
  sq0x88_t to = get_to(move);
  // castle rights
  if (from == a1 || to == a1) {
    chess_state->castle_rights &= ~WHITE_QUEEN_SIDE;
  }
  if (from == h1 || to == h1) {
    chess_state->castle_rights &= ~WHITE_KING_SIDE;
  }
  if (from == a8 || to == a8) {
    chess_state->castle_rights &= ~BLACK_QUEEN_SIDE;
  }
  if (from == h8 || to == h8) {
    chess_state->castle_rights &= ~BLACK_KING_SIDE;
  }
  if (from == e1) {
    chess_state->castle_rights &= ~WHITE_BOTH_SIDES;
  }
  if (from == e8) {
    chess_state->castle_rights &= ~BLACK_BOTH_SIDES;
  }
}

void update_enpassent_target(chess_state_t* chess_state, move_t move) {
  if (get_flags(move) == DOUBLE_PAWN_PUSH) {
    chess_state->enpassent_target = get_to(move);
  } else {
    chess_state->enpassent_target = 0x88;
  }
}

void update_half_move_clock(chess_state_t* chess_state, move_t move) {
  if (piece(chess_state, get_from(move)) & PAWN || is_capture(move)) {
    chess_state->ply_of_last_irreversible_move = chess_state->ply_counter;
    chess_state->half_move_clock = 0;
  } else {
    chess_state->half_move_clock++;
  }
}

#define SWAP(type, lhs, rhs) \
  {                          \
    type temp = (lhs);       \
    (lhs) = (rhs);           \
    (rhs) = temp;            \
  }

void update_turn(chess_state_t* chess_state) {
  chess_state->black_to_move = !chess_state->black_to_move;
  chess_state->zobrist = zobrist_flip_turn(chess_state->zobrist);
  SWAP(colour_t, chess_state->friendly_colour, chess_state->enemy_colour);
  SWAP(piece_list_t*, chess_state->friendly_pieces, chess_state->enemy_pieces);
  SWAP(sq0x88_t, chess_state->up_increment, chess_state->down_increment);
}

#include <stdio.h>
void make_move(chess_state_t* chess_state, move_t move) {
  push_ply_stack(chess_state, move);
  update_half_move_clock(chess_state, move);
  update_board(chess_state, move);
  update_enpassent_target(chess_state, move);
  update_rights(chess_state, move);
  update_turn(chess_state);
  update_check(chess_state, move);
}

// restores the board to the state before the previous move was made
void unmake_move(chess_state_t* chess_state) {
  --chess_state->ply_counter;

  chess_state->check_square =
      chess_state->ply_stack[chess_state->ply_counter].check_square;
  chess_state->discovered_check =
      chess_state->ply_stack[chess_state->ply_counter].discovered_check;
  chess_state->n_checks =
      chess_state->ply_stack[chess_state->ply_counter].n_checks;
  chess_state->enpassent_target =
      chess_state->ply_stack[chess_state->ply_counter].enpassent;
  chess_state->castle_rights =
      chess_state->ply_stack[chess_state->ply_counter].rights;
  chess_state->half_move_clock =
      chess_state->ply_stack[chess_state->ply_counter].half_move_clock;
  //chess_state->zobrist =
  //    chess_state->ply_stack[chess_state->ply_counter].zobrist;
  chess_state->ply_of_last_irreversible_move =
      chess_state->ply_stack[chess_state->ply_counter].last_irreversible;
  move_t prev_move = chess_state->ply_stack[chess_state->ply_counter].move;
  piece_t captured_piece =
      chess_state->ply_stack[chess_state->ply_counter].captured;
  update_turn(chess_state);
  sq0x88_t from = get_from(prev_move);
  sq0x88_t to = get_to(prev_move);

  if (is_promotion(prev_move)) {
    remove_piece(chess_state, to);
    place_piece(chess_state, from, chess_state->friendly_colour | PAWN);
  } else {
    move_piece(chess_state, to, from);
  }

  if (is_enpassent(prev_move)) {
    place_piece(chess_state, chess_state->enpassent_target,
                chess_state->enemy_colour | PAWN);
  } else if (is_capture(prev_move)) {
    place_piece(chess_state, to, captured_piece);
  }

  if (is_king_castle(prev_move)) {
    move_piece(chess_state, to - 1, from + 3);
  }
  if (is_queen_castle(prev_move)) {
    move_piece(chess_state, to + 1, from - 4);
  }
}
