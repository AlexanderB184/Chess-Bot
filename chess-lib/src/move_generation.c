#include <stdlib.h>

#include "../include/chess-lib.h"
#include "../include/private/chess-lib-internals.h"

// pawn helper functions

int is_promoting(const chess_state_t* chess_state, sq0x88_t from) {
  return (sq0x88_to_rank07(from) == 1 &&
          chess_state->friendly_colour == BLACK) ||
         (sq0x88_to_rank07(from) == 6 && chess_state->friendly_colour == WHITE);
}

int pawn_can_double_push(const chess_state_t* chess_state, sq0x88_t from) {
  return (sq0x88_to_rank07(from) == 6 &&
          chess_state->friendly_colour == BLACK) ||
         (sq0x88_to_rank07(from) == 1 && chess_state->friendly_colour == WHITE);
}

int can_capture_enpassent(const chess_state_t* chess_state, sq0x88_t from) {
  return sq0x88_to_rank07(from + chess_state->up_increment) ==
             sq0x88_to_rank07(enpassent_target(chess_state)) &&
         abs(sq0x88_to_file07(from) -
             sq0x88_to_file07(enpassent_target(chess_state))) == 1;
}

// king helper functions

int can_castle_king_side(const chess_state_t* chess_state) {
  sq0x88_t king_square = chess_state->friendly_pieces->king_square;
  return ((chess_state->friendly_colour == WHITE &&
           (chess_state->castle_rights & WHITE_KING_SIDE) != 0) ||
          (chess_state->friendly_colour == BLACK &&
           (chess_state->castle_rights & BLACK_KING_SIDE) != 0)) &&
         piece(chess_state, king_square + 1) == EMPTY &&
         piece(chess_state, king_square + 2) == EMPTY;
}

int can_castle_queen_side(const chess_state_t* chess_state) {
  sq0x88_t king_square = chess_state->friendly_pieces->king_square;
  return ((chess_state->friendly_colour == WHITE &&
           (chess_state->castle_rights & WHITE_QUEEN_SIDE) != 0) ||
          (chess_state->friendly_colour == BLACK &&
           (chess_state->castle_rights & BLACK_QUEEN_SIDE) != 0)) &&
         piece(chess_state, king_square - 1) == EMPTY &&
         piece(chess_state, king_square - 2) == EMPTY &&
         piece(chess_state, king_square - 3) == EMPTY;
}

// knight movement

size_t knight_moves(const chess_state_t* chess_state,
                                          move_t* moves, size_t move_count,
                                          sq0x88_t from) {
  for (int i = 0; i < knight_increments_count; i++) {
    sq0x88_t to = from + knight_increments_list[i];
    if (off_the_board(to) || piece_is_friendly(chess_state, to)) {
      continue;
    }
    piece_t target_piece = piece(chess_state, to);
    if (target_piece != EMPTY) continue;
    moves[move_count++] = move(from, to, QUIET_MOVE);
  }
  return move_count;
}

size_t knight_captures(const chess_state_t* chess_state,
                                          move_t* moves, size_t move_count,
                                          sq0x88_t from) {
  for (int i = 0; i < knight_increments_count; i++) {
    sq0x88_t to = from + knight_increments_list[i];
    if (off_the_board(to) || piece_is_friendly(chess_state, to)) {
      continue;
    }
    piece_t target_piece = piece(chess_state, to);
    if (target_piece == EMPTY) continue;
    moves[move_count++] = move(from, to, CAPTURE);
  }
  return move_count;
}

size_t knight_blocks(const chess_state_t* chess_state,
                                          move_t* moves, size_t move_count,
                                          sq0x88_t from, sq0x88_t checker, sq0x88_t king, sq0x88_t inc) {
  for (sq0x88_t target = king + inc; target != checker; target += inc) {
    if (knight_increment(from, target)) {
      moves[move_count++] = move(from, target, QUIET_MOVE);
    }
  }
  return move_count;
}

// king movement

size_t king_captures(const chess_state_t* chess_state,
                                        move_t* moves, size_t move_count,
                                        sq0x88_t king_square) {
  for (int i = 0; i < king_increments_count; i++) {
    sq0x88_t to = king_square + king_increments_list[i];
    if (off_the_board(to) || piece_is_friendly(chess_state, to)) {
      continue;
    }
    piece_t target_piece = piece(chess_state, to);
    if (target_piece == EMPTY) continue;
    moves[move_count++] = move(king_square, to, CAPTURE);
  }

  return move_count;
}

size_t king_moves(const chess_state_t* chess_state,
                                        move_t* moves, size_t move_count,
                                        sq0x88_t king_square) {
  for (int i = 0; i < king_increments_count; i++) {
    sq0x88_t to = king_square + king_increments_list[i];
    if (off_the_board(to) || piece_is_friendly(chess_state, to)) {
      continue;
    }
    piece_t target_piece = piece(chess_state, to);
    if (target_piece != EMPTY) continue;
    moves[move_count++] = move(king_square, to, QUIET_MOVE);
  }
  return move_count;
}


size_t castling_moves(const chess_state_t* chess_state, move_t* moves,
                               size_t move_count, sq0x88_t king_square) {
  // castling
  if (can_castle_king_side(chess_state)) {
    moves[move_count++] = move(king_square, king_square + 2, KING_CASTLE);
  }
  if (can_castle_queen_side(chess_state)) {
    moves[move_count++] = move(king_square, king_square - 2, QUEEN_CASTLE);
  }
  return move_count;
}

// pawn movement

size_t pawn_moves(const chess_state_t* chess_state,
                                        move_t* moves, size_t move_count,
                                        sq0x88_t from) {
  // if is promoting
  sq0x88_t inc = chess_state->up_increment;
  sq0x88_t to = from + inc;
  if (is_promoting(chess_state, from)) {
    return move_count;
  }

  if (!off_the_board(to) && piece(chess_state, to) == EMPTY) {
    moves[move_count++] = move(from, to, QUIET_MOVE);
  }
  to = from + 2 * inc;
  if (!off_the_board(to) && piece(chess_state, to) == EMPTY &&
      piece(chess_state, to - inc) == EMPTY &&
      pawn_can_double_push(chess_state, from)) {
    moves[move_count++] = move(from, to, DOUBLE_PAWN_PUSH);
  }

  return move_count;
}

size_t pawn_captures(const chess_state_t* chess_state,
                                        move_t* moves, size_t move_count,
                                        sq0x88_t from) {
  // if is promoting
  sq0x88_t inc = chess_state->up_increment;
  sq0x88_t to = from + inc;
  if (is_promoting(chess_state, from)) {
    return move_count;
  }

  to = from + inc + 1;
  if (!off_the_board(to) && piece_is_enemy(chess_state, to)) {
    moves[move_count++] = move(from, to, CAPTURE);
  }
  to = from + inc - 1;
  if (!off_the_board(to) && piece_is_enemy(chess_state, to)) {
    moves[move_count++] = move(from, to, CAPTURE);
  }

  if (!off_the_board(enpassent_target(chess_state)) &&
      can_capture_enpassent(chess_state, from)) {
    moves[move_count++] = move(from, enpassent_target(chess_state), ENPASSENT);
  }

  return move_count;
}

size_t pawn_promotions(const chess_state_t* chess_state,
                                           move_t* moves, size_t move_count,
                                           sq0x88_t from) {
  if (!is_promoting(chess_state, from)) return move_count;
  sq0x88_t inc = chess_state->up_increment;
  sq0x88_t to = from + inc;
  if (piece(chess_state, to) == EMPTY) {
    moves[move_count++] = move(from, to, QUEEN_PROMOTION);
    moves[move_count++] = move(from, to, ROOK_PROMOTION);
    moves[move_count++] = move(from, to, BISHOP_PROMOTION);
    moves[move_count++] = move(from, to, KNIGHT_PROMOTION);
    
  }
   to = from + inc + 1;
  if (!off_the_board(to) && piece_is_enemy(chess_state, to)) {
    moves[move_count++] = move(from, to, QUEEN_CAPTURE_PROMOTION);
    moves[move_count++] = move(from, to, ROOK_CAPTURE_PROMOTION);
    moves[move_count++] = move(from, to, BISHOP_CAPTURE_PROMOTION);
    moves[move_count++] = move(from, to, KNIGHT_CAPTURE_PROMOTION);
  }
  to = from + inc - 1;
  if (!off_the_board(to) && piece_is_enemy(chess_state, to)) {
    moves[move_count++] = move(from, to, QUEEN_CAPTURE_PROMOTION);
    moves[move_count++] = move(from, to, ROOK_CAPTURE_PROMOTION);
    moves[move_count++] = move(from, to, BISHOP_CAPTURE_PROMOTION);
    moves[move_count++] = move(from, to, KNIGHT_CAPTURE_PROMOTION);
  }
  return move_count;
}

size_t pawn_captures_of(const chess_state_t* chess_state, move_t* moves, size_t move_count, sq0x88_t target) {
  // pawn captures
  sq0x88_t inc = chess_state->up_increment;
  sq0x88_t from;
  piece_t friendly_pawn = chess_state->friendly_colour | PAWN;
  if (is_promoting(chess_state, target - inc)) {
    return move_count;
  }

  from = target - inc + 1;
  if (!off_the_board(from) && piece(chess_state, from) == friendly_pawn) {
    moves[move_count++] = move(from, target, CAPTURE);
  }

  from = target - inc - 1;
  if (!off_the_board(from) && piece(chess_state, from) == friendly_pawn) {
    moves[move_count++] = move(from, target, CAPTURE);
  }

  if (enpassent_target(chess_state) == target + chess_state->up_increment) {
    from = target + 1;
    if (!off_the_board(from) && piece(chess_state, from) == friendly_pawn &&
        can_capture_enpassent(chess_state, from)) {
      moves[move_count++] = move(from, enpassent_target(chess_state), ENPASSENT);
    }
    from = target - 1;
    if (!off_the_board(from) && piece(chess_state, from) == friendly_pawn &&
        can_capture_enpassent(chess_state, from)) {
      moves[move_count++] = move(from, enpassent_target(chess_state), ENPASSENT);
    }
  }

  return move_count;
}

size_t pawn_pushes_to(const chess_state_t* chess_state, move_t* moves, size_t move_count, sq0x88_t target) {
  // pawn moves
  sq0x88_t inc = chess_state->up_increment;
  sq0x88_t from;
  piece_t friendly_pawn = PAWN | chess_state->friendly_colour;
  from = target - inc;
  if (!off_the_board(from) && piece(chess_state, from) == friendly_pawn &&!is_promoting(chess_state, from)) {
    moves[move_count++] = move(from, target, QUIET_MOVE);
  }

  from = target - 2 * inc;
  if (!off_the_board(from) && piece(chess_state, from) == friendly_pawn &&
      piece(chess_state, target - inc) == EMPTY &&
      pawn_can_double_push(chess_state, from)) {
    moves[move_count++] = move(from, target, DOUBLE_PAWN_PUSH);
  }

  return move_count;
}

// sliding movement

size_t sliding_captures(const chess_state_t* chess_state,
                                           move_t* moves, size_t move_count,
                                           sq0x88_t from,
                                           const sq0x88_t* increments,
                                           int increments_count) {
  for (sq0x88_t i = 0; i < increments_count; i++) {
    sq0x88_t inc = increments[i];
    sq0x88_t to;
    for (to = from + inc; piece(chess_state, to) == EMPTY; to += inc);
    if (off_the_board(to) || piece_is_friendly(chess_state, to)) continue;

    moves[move_count++] = move(from, to, CAPTURE);
  }
  return move_count;
}

size_t sliding_moves(const chess_state_t* chess_state,
                                           move_t* moves, size_t move_count,
                                           sq0x88_t from,
                                           const sq0x88_t* increments,
                                           int increments_count) {
  for (sq0x88_t i = 0; i < increments_count; i++) {
    sq0x88_t inc = increments[i];
    sq0x88_t to;
    for (to = from + inc; piece(chess_state, to) == EMPTY; to += inc) {
      moves[move_count++] = move(from, to, QUIET_MOVE);
    }
  }
  return move_count;
}

size_t sliding_moves_blocks(const chess_state_t* chess_state,
                                           move_t* moves, size_t move_count,
                                           sq0x88_t from, sq0x88_t (*incfunc)(sq0x88_t,sq0x88_t),
                                           sq0x88_t checker, sq0x88_t king, sq0x88_t inc) {
  for (sq0x88_t target = king+inc; target != checker; target+=inc) {
    sq0x88_t pinc = incfunc(from, target);
    if (pinc && backwards_ray_cast(chess_state, target, pinc) == from) {
      moves[move_count++] = move(from, target, QUIET_MOVE);
    }
  }
  return move_count;
}

// incremental movegen

size_t generate_promotions(const chess_state_t* chess_state, move_t* moves) {
  size_t move_count = 0;
  sq0x88_t inc = chess_state->up_increment;
  piece_t friendly_pawn = chess_state->friendly_colour | PAWN;
  if (is_double_check(chess_state)) return move_count;
  if (is_check(chess_state)) {
    sq0x88_t king_square = chess_state->friendly_pieces->king_square;
    sq0x88_t check_square = checking_square(chess_state);

    if ((chess_state->friendly_colour == WHITE && sq0x88_to_rank07(check_square) != 7)
     || (chess_state->friendly_colour == BLACK && sq0x88_to_rank07(check_square) != 0)) { // back rank
      return move_count;
    }
    sq0x88_t from = check_square - inc + 1;
    if (!off_the_board(from) && piece(chess_state, from) == friendly_pawn) {
      moves[move_count++] = move(from, check_square, KNIGHT_CAPTURE_PROMOTION);
      moves[move_count++] = move(from, check_square, BISHOP_CAPTURE_PROMOTION);
      moves[move_count++] = move(from, check_square, ROOK_CAPTURE_PROMOTION);
      moves[move_count++] = move(from, check_square, QUEEN_CAPTURE_PROMOTION);
    }

    from = check_square - inc - 1;
    if (!off_the_board(from) && piece(chess_state, from) == friendly_pawn) {
      moves[move_count++] = move(from, check_square, KNIGHT_CAPTURE_PROMOTION);
      moves[move_count++] = move(from, check_square, BISHOP_CAPTURE_PROMOTION);
      moves[move_count++] = move(from, check_square, ROOK_CAPTURE_PROMOTION);
      moves[move_count++] = move(from, check_square, QUEEN_CAPTURE_PROMOTION);
    }

    sq0x88_t interpose_inc = 0;

    if (piece(chess_state, check_square) & QUEEN) {
      interpose_inc = queen_increment(king_square, check_square);
    }

    if (!interpose_inc) return move_count;

    for (sq0x88_t target = king_square + interpose_inc; target != check_square; target+=interpose_inc) {
      if (piece(chess_state, target - inc) == friendly_pawn) {
        moves[move_count++] = move(from, check_square, KNIGHT_PROMOTION);
        moves[move_count++] = move(from, check_square, BISHOP_PROMOTION);
        moves[move_count++] = move(from, check_square, ROOK_PROMOTION);
        moves[move_count++] = move(from, check_square, QUEEN_PROMOTION);
      }
    }
    return move_count;
  }
  int pawn_count = chess_state->friendly_pieces->pawn_count;
  sq0x88_t* pawn_list = chess_state->friendly_pieces->pawn_list;
  for (int p = 0; p < pawn_count; p++) {
    sq0x88_t square = pawn_list[p];
    move_count = pawn_promotions(chess_state, moves, move_count, square);
  }
  return move_count;
}

size_t generate_captures(const chess_state_t* chess_state, move_t* moves) {
  size_t move_count = 0;

  move_count = king_captures(
    chess_state, 
    moves, 
    move_count,
    chess_state->friendly_pieces->king_square
  );

  if (is_double_check(chess_state)) return move_count;

  if (is_check(chess_state)) {
    sq0x88_t check_square = checking_square(chess_state);
    // queen moves
    int queen_count = chess_state->friendly_pieces->queen_count;
    sq0x88_t* queen_list = chess_state->friendly_pieces->queen_list;
    for (int p = 0; p < queen_count; p++) {
      sq0x88_t square = queen_list[p];
      sq0x88_t inc = queen_increment(square, check_square);
      if (inc && backwards_ray_cast(chess_state, check_square, inc) == square) {
        moves[move_count++] = move(square, check_square, CAPTURE);
      }
    }
    // rook moves
    int rook_count = chess_state->friendly_pieces->rook_count;
    sq0x88_t* rook_list = chess_state->friendly_pieces->rook_list;
    for (int p = 0; p < rook_count; p++) {
      sq0x88_t square = rook_list[p];
      sq0x88_t inc = rook_increment(square, check_square);
      if (inc && backwards_ray_cast(chess_state, check_square, inc) == square) {
        moves[move_count++] = move(square, check_square, CAPTURE);
      }
    }
    int bishop_count;
    sq0x88_t* bishop_list;
    if (is_light_square(check_square)) {
      bishop_count = chess_state->friendly_pieces->light_bishop_count;
      bishop_list  = chess_state->friendly_pieces->light_bishop_list;
    } else {
      bishop_count = chess_state->friendly_pieces->dark_bishop_count;
      bishop_list  = chess_state->friendly_pieces->dark_bishop_list;
    }
    for (int p = 0; p < bishop_count; p++) {
      sq0x88_t square = bishop_list[p];
      sq0x88_t inc = bishop_increment(square, check_square);
      if (inc && backwards_ray_cast(chess_state, check_square, inc) == square) {
        moves[move_count++] = move(square, check_square, CAPTURE);
      }
    }

    // knight moves

    int knight_count = chess_state->friendly_pieces->knight_count;
    sq0x88_t* knight_list = chess_state->friendly_pieces->knight_list;
    for (int p = 0; p < knight_count; p++) {
      sq0x88_t square = knight_list[p];
      sq0x88_t inc = knight_increment(square, check_square);
       if (inc) {
        moves[move_count++] = move(square, check_square, CAPTURE);
      }
    }
    

    move_count = pawn_captures_of(chess_state, moves, move_count, check_square);

    return move_count;
  }

  // queen moves
  int queen_count = chess_state->friendly_pieces->queen_count;
  sq0x88_t* queen_list = chess_state->friendly_pieces->queen_list;
  for (int p = 0; p < queen_count; p++) {
    sq0x88_t square = queen_list[p];
    move_count = sliding_captures(
      chess_state, 
      moves, move_count, 
      square, 
      queen_increments_list, queen_increments_count
    );
  }
  // rook moves
  int rook_count = chess_state->friendly_pieces->rook_count;
  sq0x88_t* rook_list = chess_state->friendly_pieces->rook_list;
  for (int p = 0; p < rook_count; p++) {
    sq0x88_t square = rook_list[p];
    move_count = sliding_captures(
      chess_state, 
      moves, move_count, 
      square, 
      rook_increments_list, rook_increments_count
    );
  }

  // rook moves
  int bishop_count = chess_state->friendly_pieces->light_bishop_count;
  sq0x88_t* bishop_list = chess_state->friendly_pieces->light_bishop_list;
  for (int p = 0; p < bishop_count; p++) {
    sq0x88_t square = bishop_list[p];
    move_count = sliding_captures(
      chess_state, 
      moves, move_count, 
      square, 
      bishop_increments_list, bishop_increments_count
    );
  }

  bishop_count = chess_state->friendly_pieces->dark_bishop_count;
  bishop_list = chess_state->friendly_pieces->dark_bishop_list;
  for (int p = 0; p < bishop_count; p++) {
    sq0x88_t square = bishop_list[p];
    move_count = sliding_captures(
      chess_state, 
      moves, move_count, 
      square, 
      bishop_increments_list, bishop_increments_count
    );
  }

  // knight moves

  int knight_count = chess_state->friendly_pieces->knight_count;
  sq0x88_t* knight_list = chess_state->friendly_pieces->knight_list;
  for (int p = 0; p < knight_count; p++) {
    sq0x88_t square = knight_list[p];
    move_count = knight_captures(chess_state, moves, move_count, square);
  }

  int pawn_count = chess_state->friendly_pieces->pawn_count;
  sq0x88_t* pawn_list = chess_state->friendly_pieces->pawn_list;
  for (int p = 0; p < pawn_count; p++) {
    sq0x88_t square = pawn_list[p];
    move_count = pawn_captures(chess_state, moves, move_count, square);
  }

  return move_count;
}

size_t generate_quiets(const chess_state_t* chess_state, move_t* moves) {
  size_t move_count = 0;

  move_count = king_moves(
    chess_state, 
    moves, 
    move_count,
    chess_state->friendly_pieces->king_square
  );

  if (is_double_check(chess_state)) return move_count;

  if (is_check(chess_state)) {
    sq0x88_t check_square = checking_square(chess_state);
    sq0x88_t king_square = chess_state->friendly_pieces->king_square;

    piece_t checking_piece = piece(chess_state, check_square);

    if ((checking_piece & QUEEN) == 0) {
      return move_count;
    }
    sq0x88_t inc = queen_increment(king_square, check_square); 

    // queen moves
    int queen_count = chess_state->friendly_pieces->queen_count;
    sq0x88_t* queen_list = chess_state->friendly_pieces->queen_list;
    for (int p = 0; p < queen_count; p++) {
      sq0x88_t square = queen_list[p];
      move_count = sliding_moves_blocks(
        chess_state,
        moves, move_count,
        square, queen_increment,
        check_square, king_square, inc
      );
    }
    // rook moves
    int rook_count = chess_state->friendly_pieces->rook_count;
    sq0x88_t* rook_list = chess_state->friendly_pieces->rook_list;
    for (int p = 0; p < rook_count; p++) {
      sq0x88_t square = rook_list[p];
      move_count = sliding_moves_blocks(
        chess_state,
        moves, move_count,
        square, rook_increment,
        check_square, king_square, inc
      );
    }

    // rook moves
    int bishop_count = chess_state->friendly_pieces->light_bishop_count;
    sq0x88_t* bishop_list = chess_state->friendly_pieces->light_bishop_list;
    for (int p = 0; p < bishop_count; p++) {
      sq0x88_t square = bishop_list[p];
      move_count = sliding_moves_blocks(
        chess_state,
        moves, move_count,
        square, bishop_increment,
        check_square, king_square, inc
      );
    }

    bishop_count = chess_state->friendly_pieces->dark_bishop_count;
    bishop_list = chess_state->friendly_pieces->dark_bishop_list;
    for (int p = 0; p < bishop_count; p++) {
      sq0x88_t square = bishop_list[p];
      move_count = sliding_moves_blocks(
        chess_state,
        moves, move_count,
        square, bishop_increment,
        check_square, king_square, inc
      );
    }

    // knight moves

    int knight_count = chess_state->friendly_pieces->knight_count;
    sq0x88_t* knight_list = chess_state->friendly_pieces->knight_list;
    for (int p = 0; p < knight_count; p++) {
      sq0x88_t square = knight_list[p];
      move_count = knight_blocks(
        chess_state,
        moves, move_count,
        square, check_square, king_square, inc
      );
    }


    for (sq0x88_t target = king_square + inc; target != check_square;
          target += inc) {
      move_count = pawn_pushes_to(chess_state, moves, move_count, target);
    }
    
    return move_count;
  }

  move_count = castling_moves(chess_state, moves, move_count, chess_state->friendly_pieces->king_square);

  // queen moves
  int queen_count = chess_state->friendly_pieces->queen_count;
  sq0x88_t* queen_list = chess_state->friendly_pieces->queen_list;
  for (int p = 0; p < queen_count; p++) {
    sq0x88_t square = queen_list[p];
    move_count = sliding_moves(
      chess_state, 
      moves, move_count, 
      square, 
      queen_increments_list, queen_increments_count
    );
  }
  // rook moves
  int rook_count = chess_state->friendly_pieces->rook_count;
  sq0x88_t* rook_list = chess_state->friendly_pieces->rook_list;
  for (int p = 0; p < rook_count; p++) {
    sq0x88_t square = rook_list[p];
    move_count = sliding_moves(
      chess_state, 
      moves, move_count, 
      square, 
      rook_increments_list, rook_increments_count
    );
  }

  // rook moves
  int bishop_count = chess_state->friendly_pieces->light_bishop_count;
  sq0x88_t* bishop_list = chess_state->friendly_pieces->light_bishop_list;
  for (int p = 0; p < bishop_count; p++) {
    sq0x88_t square = bishop_list[p];
    move_count = sliding_moves(
      chess_state, 
      moves, move_count, 
      square, 
      bishop_increments_list, bishop_increments_count
    );
  }

  bishop_count = chess_state->friendly_pieces->dark_bishop_count;
  bishop_list = chess_state->friendly_pieces->dark_bishop_list;
  for (int p = 0; p < bishop_count; p++) {
    sq0x88_t square = bishop_list[p];
    move_count = sliding_moves(
      chess_state, 
      moves, move_count, 
      square, 
      bishop_increments_list, bishop_increments_count
    );
  }

  // knight moves

  int knight_count = chess_state->friendly_pieces->knight_count;
  sq0x88_t* knight_list = chess_state->friendly_pieces->knight_list;
  for (int p = 0; p < knight_count; p++) {
    sq0x88_t square = knight_list[p];
    move_count = knight_moves(chess_state, moves, move_count, square);
  }

  int pawn_count = chess_state->friendly_pieces->pawn_count;
  sq0x88_t* pawn_list = chess_state->friendly_pieces->pawn_list;
  for (int p = 0; p < pawn_count; p++) {
    sq0x88_t square = pawn_list[p];
    move_count = pawn_moves(chess_state, moves, move_count, square);
  }

  return move_count;
}

size_t generate_moves(const chess_state_t* chess_state, move_t* moves) {
  size_t move_count = 0;
  move_count += generate_quiets(chess_state, moves + move_count);
  move_count += generate_captures(chess_state, moves + move_count);
  move_count += generate_promotions(chess_state, moves + move_count);
  return move_count;
}

size_t make_legal(const chess_state_t* chess_state, move_t* moves, size_t move_count) {
  for (size_t i = 0; i < move_count; i++) {
    if (!is_legal(chess_state, moves[i])) {
      moves[i--] = moves[--move_count];
    }
  }
  return move_count;
}

size_t generate_legal_promotions(const chess_state_t* chess_state, move_t* moves) {
  size_t move_count = generate_promotions(chess_state, moves);
  return make_legal(chess_state, moves, move_count);
}

size_t generate_legal_captures(const chess_state_t* chess_state, move_t* moves) {
  size_t move_count = generate_captures(chess_state, moves);
  return make_legal(chess_state, moves, move_count);
}

size_t generate_legal_quiets(const chess_state_t* chess_state, move_t* moves) {
  size_t move_count = generate_quiets(chess_state, moves);
  return make_legal(chess_state, moves, move_count);
}

size_t generate_legal_moves(const chess_state_t* chess_state, move_t* moves) {
  size_t move_count = generate_moves(chess_state, moves);
  return make_legal(chess_state, moves, move_count);
}
