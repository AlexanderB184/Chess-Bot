
#include <stdlib.h>

#include "../include/chess-lib.h"
#include "../include/private/chess-lib-internals.h"

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

int can_capture_enpassent(sq0x88_t from, sq0x88_t enpassent_target) {
  return sq0x88_to_rank07(from) == sq0x88_to_rank07(enpassent_target) &&
         abs(sq0x88_to_file07(from) - sq0x88_to_file07(enpassent_target)) == 1;
}

size_t generate_psuedo_legal_knight_moves(const chess_state_t* chess_state,
                                          move_t* moves, size_t move_count,
                                          sq0x88_t from) {
  for (int i = 0; i < knight_increments_count; i++) {
    sq0x88_t to = from + knight_increments_list[i];
    if (off_the_board(to)) {
      continue;
    }
    if (piece_is_friendly(chess_state, to)) {
      continue;
    }
    piece_t target_piece = piece(chess_state, to);
    move_flags_t flags = (target_piece == EMPTY) ? (QUIET_MOVE) : (CAPTURE);
    moves[move_count++] = move(
        from, to, flags, KNIGHT | chess_state->friendly_colour, target_piece);
  }
  return move_count;
}

size_t generate_psuedo_legal_king_moves(const chess_state_t* chess_state,
                                        move_t* moves, size_t move_count,
                                        sq0x88_t king_square) {
  for (int i = 0; i < king_increments_count; i++) {
    sq0x88_t to = king_square + king_increments_list[i];
    if (off_the_board(to)) {
      continue;
    }
    piece_t target_piece = piece(chess_state, to);
    if (target_piece & chess_state->friendly_colour) {
      continue;
    }
    move_flags_t flags = (target_piece == EMPTY) ? (QUIET_MOVE) : (CAPTURE);
    moves[move_count++] =
        move(king_square, to, flags, KING | chess_state->friendly_colour,
             target_piece);
  }

  return move_count;
}

size_t generate_castling_moves(const chess_state_t* chess_state, move_t* moves,
                               size_t move_count, sq0x88_t king_square) {
  // castling
  if ((chess_state->friendly_colour == WHITE &&
       (chess_state->castle_rights & WHITE_KING_SIDE)) ||
      (chess_state->friendly_colour == BLACK &&
       (chess_state->castle_rights & BLACK_KING_SIDE))) {
    if (piece(chess_state, king_square + 1) == EMPTY &&
        piece(chess_state, king_square + 2) == EMPTY) {
      moves[move_count++] = move(king_square, king_square + 2, KING_CASTLE,
                                 KING | chess_state->friendly_colour, EMPTY);
    }
  }
  if ((chess_state->friendly_colour == WHITE &&
       (chess_state->castle_rights & WHITE_QUEEN_SIDE)) ||
      (chess_state->friendly_colour == BLACK &&
       (chess_state->castle_rights & BLACK_QUEEN_SIDE))) {
    if (piece(chess_state, king_square - 1) == EMPTY &&
        piece(chess_state, king_square - 2) == EMPTY &&
        piece(chess_state, king_square - 3) == EMPTY) {
      moves[move_count++] = move(king_square, king_square - 2, QUEEN_CASTLE,
                                 KING | chess_state->friendly_colour, EMPTY);
    }
  }
  return move_count;
}

#include <stdio.h>
size_t generate_psuedo_legal_pawn_moves(const chess_state_t* chess_state,
                                        move_t* moves, size_t move_count,
                                        sq0x88_t from) {
  // if is promoting
  piece_t this_piece = PAWN | chess_state->friendly_colour;
  sq0x88_t inc = chess_state->up_increment;
  sq0x88_t to = from + inc;
  if (is_promoting(chess_state, from)) {
    if (piece(chess_state, to) == EMPTY) {
      moves[move_count++] = move(from, to, QUEEN_PROMOTION, this_piece, EMPTY);
      moves[move_count++] = move(from, to, ROOK_PROMOTION, this_piece, EMPTY);
      moves[move_count++] = move(from, to, BISHOP_PROMOTION, this_piece, EMPTY);
      moves[move_count++] = move(from, to, KNIGHT_PROMOTION, this_piece, EMPTY);
    }
    to = from + inc + 1;
    if (!off_the_board(to) && piece_is_enemy(chess_state, to)) {
      piece_t captured_piece = piece(chess_state, to);
      moves[move_count++] =
          move(from, to, QUEEN_CAPTURE_PROMOTION, this_piece, captured_piece);
      moves[move_count++] =
          move(from, to, ROOK_CAPTURE_PROMOTION, this_piece, captured_piece);
      moves[move_count++] =
          move(from, to, BISHOP_CAPTURE_PROMOTION, this_piece, captured_piece);
      moves[move_count++] =
          move(from, to, KNIGHT_CAPTURE_PROMOTION, this_piece, captured_piece);
    }
    to = from + inc - 1;
    if (!off_the_board(to) && piece_is_enemy(chess_state, to)) {
      piece_t captured_piece = piece(chess_state, to);
      moves[move_count++] =
          move(from, to, QUEEN_CAPTURE_PROMOTION, this_piece, captured_piece);
      moves[move_count++] =
          move(from, to, ROOK_CAPTURE_PROMOTION, this_piece, captured_piece);
      moves[move_count++] =
          move(from, to, BISHOP_CAPTURE_PROMOTION, this_piece, captured_piece);
      moves[move_count++] =
          move(from, to, KNIGHT_CAPTURE_PROMOTION, this_piece, captured_piece);
    }
    return move_count;
  }

  if (!off_the_board(to) && piece(chess_state, to) == EMPTY) {
    moves[move_count++] = move(from, to, QUIET_MOVE, this_piece, EMPTY);
  }
  to = from + inc + 1;
  if (!off_the_board(to) && piece_is_enemy(chess_state, to)) {
    moves[move_count++] =
        move(from, to, CAPTURE, this_piece, piece(chess_state, to));
  }
  to = from + inc - 1;
  if (!off_the_board(to) && piece_is_enemy(chess_state, to)) {
    moves[move_count++] =
        move(from, to, CAPTURE, this_piece, piece(chess_state, to));
  }
  to = from + 2 * inc;
  if (!off_the_board(to) && piece(chess_state, to) == EMPTY &&
      piece(chess_state, to - inc) == EMPTY &&
      pawn_can_double_push(chess_state, from)) {
    moves[move_count++] = move(from, to, DOUBLE_PAWN_PUSH, this_piece, EMPTY);
  }

  if (!off_the_board(enpassent_target(chess_state)) &&
      can_capture_enpassent(from, enpassent_target(chess_state))) {
    moves[move_count++] =
        move(from, enpassent_target(chess_state) + inc, ENPASSENT, this_piece,
             PAWN | chess_state->enemy_colour);
  }

  return move_count;
}

size_t generate_psuedo_legal_sliding_moves(const chess_state_t* chess_state,
                                           move_t* moves, size_t move_count,
                                           sq0x88_t from,
                                           const sq0x88_t* increments,
                                           int increments_count) {
  piece_t this_piece = piece(chess_state, from);
  for (sq0x88_t i = 0; i < increments_count; i++) {
    sq0x88_t inc = increments[i];
    sq0x88_t to;
    for (to = from + inc; !off_the_board(to) && piece(chess_state, to) == EMPTY;
         to += inc) {
      moves[move_count++] = move(from, to, QUIET_MOVE, this_piece, EMPTY);
    }
    if (off_the_board(to) || piece_is_friendly(chess_state, to)) continue;

    moves[move_count++] =
        move(from, to, CAPTURE, this_piece, piece(chess_state, to));
  }
  return move_count;
}

size_t generate_psuedo_legal_moves(const chess_state_t* chess_state,
                                   move_t* moves) {
  size_t move_count = 0;

  // king moves
  move_count = generate_psuedo_legal_king_moves(
      chess_state, moves, move_count,
      chess_state->friendly_pieces->king_square);

  move_count =
      generate_castling_moves(chess_state, moves, move_count,
                              chess_state->friendly_pieces->king_square);

  // queen moves
  for (int queen_index = 0;
       queen_index < chess_state->friendly_pieces->queen_count; queen_index++) {
    move_count = generate_psuedo_legal_sliding_moves(
        chess_state, moves, move_count,
        chess_state->friendly_pieces->queen_list[queen_index],
        queen_increments_list, queen_increments_count);
  }
  // rook moves
  for (int rook_index = 0;
       rook_index < chess_state->friendly_pieces->rook_count; rook_index++) {
    move_count = generate_psuedo_legal_sliding_moves(
        chess_state, moves, move_count,
        chess_state->friendly_pieces->rook_list[rook_index],
        rook_increments_list, rook_increments_count);
  }

  // light bishop moves
  for (int light_bishop_index = 0;
       light_bishop_index < chess_state->friendly_pieces->light_bishop_count;
       light_bishop_index++) {
    move_count = generate_psuedo_legal_sliding_moves(
        chess_state, moves, move_count,
        chess_state->friendly_pieces->light_bishop_list[light_bishop_index],
        bishop_increments_list, bishop_increments_count);
  }

  // dark bishop moves
  for (int dark_bishop_index = 0;
       dark_bishop_index < chess_state->friendly_pieces->dark_bishop_count;
       dark_bishop_index++) {
    move_count = generate_psuedo_legal_sliding_moves(
        chess_state, moves, move_count,
        chess_state->friendly_pieces->dark_bishop_list[dark_bishop_index],
        bishop_increments_list, bishop_increments_count);
  }

  // knight moves
  for (int knight_index = 0;
       knight_index < chess_state->friendly_pieces->knight_count;
       knight_index++) {
    sq0x88_t from = chess_state->friendly_pieces->knight_list[knight_index];
    move_count = generate_psuedo_legal_knight_moves(chess_state, moves,
                                                    move_count, from);
  }

  // pawn moves
  for (int pawn_index = 0;
       pawn_index < chess_state->friendly_pieces->pawn_count; pawn_index++) {
    sq0x88_t from = chess_state->friendly_pieces->pawn_list[pawn_index];
    move_count =
        generate_psuedo_legal_pawn_moves(chess_state, moves, move_count, from);
  }

  return move_count;
}

size_t generate_sliding_piece_capture(const chess_state_t* chess_state,
                                      move_t* moves, size_t move_count,
                                      sq0x88_t square, sq0x88_t target,
                                      sq0x88_t inc) {
  if (inc && forwards_ray_cast(chess_state, square, inc) == target) {
    moves[move_count++] =
        move(square, target, CAPTURE, piece(chess_state, square),
             piece(chess_state, target));
  }
  return move_count;
}

size_t generate_sliding_piece_interposing_move(const chess_state_t* chess_state,
                                               move_t* moves, size_t move_count,
                                               sq0x88_t square, sq0x88_t target,
                                               sq0x88_t inc) {
  if (inc && backwards_ray_cast(chess_state, target, inc) == square) {
    moves[move_count++] =
        move(square, target, QUIET_MOVE, piece(chess_state, square), EMPTY);
  }
  return move_count;
}

size_t generate_psuedo_legal_captures_of(const chess_state_t* chess_state,
                                         move_t* moves, size_t move_count,
                                         sq0x88_t target) {
  piece_t captured_piece = piece(chess_state, target);

  // queen moves
  for (int p = 0; p < chess_state->friendly_pieces->queen_count; p++) {
    move_count = generate_sliding_piece_capture(
        chess_state, moves, move_count,
        chess_state->friendly_pieces->queen_list[p], target,
        queen_increment(chess_state->friendly_pieces->queen_list[p], target));
  }

  // rook moves
  for (int p = 0; p < chess_state->friendly_pieces->rook_count; p++) {
    move_count = generate_sliding_piece_capture(
        chess_state, moves, move_count,
        chess_state->friendly_pieces->rook_list[p], target,
        rook_increment(chess_state->friendly_pieces->rook_list[p], target));
  }

  if (is_light_square(target)) {
    // light bishop moves
    for (int p = 0; p < chess_state->friendly_pieces->light_bishop_count; p++) {
      move_count = generate_sliding_piece_capture(
          chess_state, moves, move_count,
          chess_state->friendly_pieces->light_bishop_list[p], target,
          bishop_increment(chess_state->friendly_pieces->light_bishop_list[p],
                           target));
    }
  } else {
    // dark bishop moves
    for (int p = 0; p < chess_state->friendly_pieces->dark_bishop_count; p++) {
      move_count = generate_sliding_piece_capture(
          chess_state, moves, move_count,
          chess_state->friendly_pieces->dark_bishop_list[p], target,
          bishop_increment(chess_state->friendly_pieces->dark_bishop_list[p],
                           target));
    }
  }

  // knight moves
  for (int knight_index = 0;
       knight_index < chess_state->friendly_pieces->knight_count;
       knight_index++) {
    if (knight_increment(
            chess_state->friendly_pieces->knight_list[knight_index], target)) {
      moves[move_count++] =
          move(chess_state->friendly_pieces->knight_list[knight_index], target,
               CAPTURE, KNIGHT | chess_state->friendly_colour,
               piece(chess_state, target));
    }
  }

  // pawn captures
  sq0x88_t inc = chess_state->up_increment;
  sq0x88_t from;
  piece_t friendly_pawn = chess_state->friendly_colour | PAWN;
  if (is_promoting(chess_state, target - inc)) {
    from = target - inc + 1;
    if (!off_the_board(from) && piece(chess_state, from) == friendly_pawn) {
      moves[move_count++] = move(from, target, QUEEN_CAPTURE_PROMOTION,
                                 friendly_pawn, captured_piece);
      moves[move_count++] = move(from, target, ROOK_CAPTURE_PROMOTION,
                                 friendly_pawn, captured_piece);
      moves[move_count++] = move(from, target, BISHOP_CAPTURE_PROMOTION,
                                 friendly_pawn, captured_piece);
      moves[move_count++] = move(from, target, KNIGHT_CAPTURE_PROMOTION,
                                 friendly_pawn, captured_piece);
    }
    from = target - inc - 1;
    if (!off_the_board(from) && piece(chess_state, from) == friendly_pawn) {
      moves[move_count++] = move(from, target, QUEEN_CAPTURE_PROMOTION,
                                 friendly_pawn, captured_piece);
      moves[move_count++] = move(from, target, ROOK_CAPTURE_PROMOTION,
                                 friendly_pawn, captured_piece);
      moves[move_count++] = move(from, target, BISHOP_CAPTURE_PROMOTION,
                                 friendly_pawn, captured_piece);
      moves[move_count++] = move(from, target, KNIGHT_CAPTURE_PROMOTION,
                                 friendly_pawn, captured_piece);
    }
    return move_count;
  }

  from = target - inc + 1;
  if (!off_the_board(from) && piece(chess_state, from) == friendly_pawn) {
    moves[move_count++] =
        move(from, target, CAPTURE, friendly_pawn, captured_piece);
  }

  from = target - inc - 1;
  if (!off_the_board(from) && piece(chess_state, from) == friendly_pawn) {
    moves[move_count++] =
        move(from, target, CAPTURE, friendly_pawn, captured_piece);
  }

  if (enpassent_target(chess_state) == target) {
    from = target + 1;
    if (!off_the_board(from) && piece(chess_state, from) == friendly_pawn &&
        can_capture_enpassent(from, target)) {
      moves[move_count++] =
          move(from, target + inc, ENPASSENT, friendly_pawn, captured_piece);
    }
    from = target - 1;
    if (!off_the_board(from) && piece(chess_state, from) == friendly_pawn &&
        can_capture_enpassent(from, target)) {
      moves[move_count++] =
          move(from, target + inc, ENPASSENT, friendly_pawn, captured_piece);
    }
  }

  return move_count;
}

size_t generate_psuedo_legal_interposing_moves(const chess_state_t* chess_state,
                                               move_t* moves, size_t move_count,
                                               sq0x88_t interposing_square) {
  // queen moves
  for (int p = 0; p < chess_state->friendly_pieces->queen_count; p++) {
    move_count = generate_sliding_piece_interposing_move(
        chess_state, moves, move_count,
        chess_state->friendly_pieces->queen_list[p], interposing_square,
        queen_increment(chess_state->friendly_pieces->queen_list[p],
                        interposing_square));
  }

  // rook moves
  for (int p = 0; p < chess_state->friendly_pieces->rook_count; p++) {
    move_count = generate_sliding_piece_interposing_move(
        chess_state, moves, move_count,
        chess_state->friendly_pieces->rook_list[p], interposing_square,
        rook_increment(chess_state->friendly_pieces->rook_list[p],
                       interposing_square));
  }

  if (is_light_square(interposing_square)) {
    // light bishop moves
    for (int p = 0; p < chess_state->friendly_pieces->light_bishop_count; p++) {
      move_count = generate_sliding_piece_interposing_move(
          chess_state, moves, move_count,
          chess_state->friendly_pieces->light_bishop_list[p],
          interposing_square,
          bishop_increment(chess_state->friendly_pieces->light_bishop_list[p],
                           interposing_square));
    }
  } else {
    // dark bishop moves
    for (int p = 0; p < chess_state->friendly_pieces->dark_bishop_count; p++) {
      move_count = generate_sliding_piece_interposing_move(
          chess_state, moves, move_count,
          chess_state->friendly_pieces->dark_bishop_list[p], interposing_square,
          bishop_increment(chess_state->friendly_pieces->dark_bishop_list[p],
                           interposing_square));
    }
  }

  // knight moves
  for (int knight_index = 0;
       knight_index < chess_state->friendly_pieces->knight_count;
       knight_index++) {
    if (knight_increment(
            chess_state->friendly_pieces->knight_list[knight_index],
            interposing_square)) {
      moves[move_count++] =
          move(chess_state->friendly_pieces->knight_list[knight_index],
               interposing_square, QUIET_MOVE,
               KNIGHT | chess_state->friendly_colour, EMPTY);
    }
  }

  // pawn moves
  sq0x88_t inc = chess_state->up_increment;
  sq0x88_t from;
  piece_t friendly_pawn = PAWN | chess_state->friendly_colour;
  from = interposing_square - inc;
  if (!off_the_board(from) && piece(chess_state, from) == friendly_pawn) {
    if (is_promoting(chess_state, from)) {
      moves[move_count++] =
          move(from, interposing_square, QUEEN_PROMOTION, friendly_pawn, EMPTY);
      moves[move_count++] =
          move(from, interposing_square, ROOK_PROMOTION, friendly_pawn, EMPTY);
      moves[move_count++] = move(from, interposing_square, BISHOP_PROMOTION,
                                 friendly_pawn, EMPTY);
      moves[move_count++] = move(from, interposing_square, KNIGHT_PROMOTION,
                                 friendly_pawn, EMPTY);
    } else {
      moves[move_count++] =
          move(from, interposing_square, QUIET_MOVE, friendly_pawn, EMPTY);
    }
  }

  from = interposing_square - 2 * inc;
  if (!off_the_board(from) && piece(chess_state, from) == friendly_pawn &&
      piece(chess_state, interposing_square - inc) == EMPTY &&
      pawn_can_double_push(chess_state, from)) {
    moves[move_count++] =
        move(from, interposing_square, DOUBLE_PAWN_PUSH, friendly_pawn, EMPTY);
  }

  return move_count;
}

size_t generate_check_evasions(const chess_state_t* chess_state,
                               move_t* moves) {
  // if more than 1 attacker, only king moves
  // if only 1 attacker, capture of attacker, block of attacker, king moves
  size_t move_count = 0;

  sq0x88_t king_square = chess_state->friendly_pieces->king_square;

  move_count = generate_psuedo_legal_king_moves(chess_state, moves, move_count,
                                                king_square);

  if (is_double_check(chess_state)) {
    return move_count;
  }

  piece_t checking_piece = piece(chess_state, checking_square(chess_state));

  move_count = generate_psuedo_legal_captures_of(chess_state, moves, move_count,
                                                 checking_square(chess_state));

  // if checking piece isn't a sliding piece cant be interposed
  if (!(checking_piece & (BISHOP | ROOK | QUEEN))) {
    return move_count;
  }

  sq0x88_t inc = queen_increment(king_square, checking_square(chess_state));
  // move along interpose line and find all interposing moves
  for (sq0x88_t interposing_square = king_square + inc;
       interposing_square != checking_square(chess_state);
       interposing_square += inc) {
    move_count = generate_psuedo_legal_interposing_moves(
        chess_state, moves, move_count, interposing_square);
  }

  return move_count;
}

size_t generate_legal_moves(const chess_state_t* chess_state, move_t* moves) {
  size_t move_count = generate_moves(chess_state, moves);

  for (size_t i = 0; i < move_count; i++) {
    if (!is_legal(chess_state, moves[i])) {
      moves[i--] = moves[--move_count];
    }
  }

  return move_count;
}

size_t generate_moves(const chess_state_t* chess_state, move_t* moves) {
  size_t move_count;

  if (is_check(chess_state)) {
    move_count = generate_check_evasions(chess_state, moves);
  } else {
    move_count = generate_psuedo_legal_moves(chess_state, moves);
  }
  
  return move_count;
}