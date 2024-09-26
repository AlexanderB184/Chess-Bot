#include "../include/static-evaluation.h"

#include "../include/static-evaluation-tables.h"

score_centipawn_t static_evaluation(const chess_state_t* chess_state) {
  if (is_draw(chess_state)) {
    return DRAW_SCORE_CENTIPAWNS;
  } /*
   if (is_stalemate(chess_state)) {
     return STALEMATE_SCORE_CENTIPAWNS;
   }
   if (is_checkmate(chess_state)) {
     return CHECKMATE_SCORE_CENTIPAWNS;
   }*/
  score_centipawn_t score =
      material_score(chess_state) + pawn_weakness_score(chess_state) /* +
       mobility_score(chess_state) +
       pawn_weakness_score(chess_state)*/
      ;
  return score;
}

// todo seperate midgame and endgame piece-square tables
// change evaluation function for endgames?

sq0x88_t flipped_square(sq0x88_t square) {
  return rankfile_to_sq0x88(7 - sq0x88_to_rank07(square),
                            sq0x88_to_file07(square));
}

score_centipawn_t board_value(const chess_state_t* chess_state,
                              sq0x88_t square) {
  return piece_value(square, piece(chess_state, square));
}

score_centipawn_t piece_value(sq0x88_t square, piece_t piece) {
  switch (piece & PIECE_MASK) {
    case PAWN:
      return pawn_value(square, piece);
    case KNIGHT:
      return knight_value(square, piece);
    case BISHOP:
      return bishop_value(square, piece);
    case ROOK:
      return rook_value(square, piece);
    case QUEEN:
      return queen_value(square, piece);
    case KING:
      return king_value(square, piece);
    default:
      return 0;
  }
}

score_centipawn_t value_from_piece_square_table(
    int mat_count, score_centipawn_t* mid_game_piece_square_table,
    score_centipawn_t* end_game_piece_square_table, sq0x88_t square) {
  int max_mat_count = 12 + 12 + 20 + 18;
  sq8x8_t index = sq0x88_to_sq8x8(square);
  return (mid_game_piece_square_table[index] * mat_count / max_mat_count) +
         (end_game_piece_square_table[index] * (max_mat_count - mat_count) /
          max_mat_count);
}

score_centipawn_t pawn_value(sq0x88_t square, piece_t colour) {
  if (colour & WHITE) square = flipped_square(square);
  return pawn_square_tables[0][sq0x88_to_sq8x8(square)];
}

score_centipawn_t knight_value(sq0x88_t square, piece_t colour) {
  if (colour & WHITE) square = flipped_square(square);
  return knight_square_tables[0][sq0x88_to_sq8x8(square)];
}

score_centipawn_t bishop_value(sq0x88_t square, piece_t colour) {
  if (colour & WHITE) square = flipped_square(square);
  return bishop_square_tables[0][sq0x88_to_sq8x8(square)];
}

score_centipawn_t rook_value(sq0x88_t square, piece_t colour) {
  if (colour & WHITE) square = flipped_square(square);
  return rook_square_tables[0][sq0x88_to_sq8x8(square)];
}

score_centipawn_t queen_value(sq0x88_t square, piece_t colour) {
  if (colour & WHITE) square = flipped_square(square);
  return queen_square_tables[0][sq0x88_to_sq8x8(square)];
}

score_centipawn_t king_value(sq0x88_t square, piece_t colour) {
  if (colour & WHITE) square = flipped_square(square);
  return king_square_tables[0][sq0x88_to_sq8x8(square)];
}

score_centipawn_t material_score(const chess_state_t* chess_state) {
  score_centipawn_t score = 0;
  int mat_count = 0;
  mat_count += 3 * chess_state->white_pieces.knight_count;
  mat_count += 3 * chess_state->black_pieces.knight_count;
  mat_count += 3 * chess_state->white_pieces.light_bishop_count;
  mat_count += 3 * chess_state->black_pieces.light_bishop_count;
  mat_count += 3 * chess_state->white_pieces.dark_bishop_count;
  mat_count += 3 * chess_state->black_pieces.dark_bishop_count;
  mat_count += 5 * chess_state->white_pieces.rook_count;
  mat_count += 5 * chess_state->black_pieces.rook_count;
  mat_count += 9 * chess_state->white_pieces.queen_count;
  mat_count += 9 * chess_state->black_pieces.queen_count;

  {
    sq0x88_t sq = chess_state->white_pieces.king_square;
    score += value_from_piece_square_table(mat_count, king_square_tables[0],
                                           king_square_tables[1],
                                           flipped_square(sq));
  }
  {
    sq0x88_t sq = chess_state->black_pieces.king_square;
    score -= value_from_piece_square_table(mat_count, king_square_tables[0],
                                           king_square_tables[1], sq);
  }
  for (int p = 0; p < chess_state->white_pieces.pawn_count; p++) {
    sq0x88_t sq = chess_state->white_pieces.pawn_list[p];
    score += value_from_piece_square_table(mat_count, pawn_square_tables[0],
                                           pawn_square_tables[1],
                                           flipped_square(sq));
  }
  for (int p = 0; p < chess_state->black_pieces.pawn_count; p++) {
    sq0x88_t sq = chess_state->black_pieces.pawn_list[p];
    score -= value_from_piece_square_table(mat_count, pawn_square_tables[0],
                                           pawn_square_tables[1], sq);
  }
  for (int p = 0; p < chess_state->white_pieces.knight_count; p++) {
    sq0x88_t sq = chess_state->white_pieces.knight_list[p];
    score += value_from_piece_square_table(mat_count, knight_square_tables[0],
                                           knight_square_tables[1],
                                           flipped_square(sq));
  }
  for (int p = 0; p < chess_state->black_pieces.knight_count; p++) {
    sq0x88_t sq = chess_state->black_pieces.knight_list[p];
    score -= value_from_piece_square_table(mat_count, knight_square_tables[0],
                                           knight_square_tables[1], sq);
  }
  for (int p = 0; p < chess_state->white_pieces.light_bishop_count; p++) {
    sq0x88_t sq = chess_state->white_pieces.light_bishop_list[p];
    score += value_from_piece_square_table(mat_count, bishop_square_tables[0],
                                           bishop_square_tables[1],
                                           flipped_square(sq));
  }
  for (int p = 0; p < chess_state->black_pieces.light_bishop_count; p++) {
    sq0x88_t sq = chess_state->black_pieces.light_bishop_list[p];
    score -= value_from_piece_square_table(mat_count, bishop_square_tables[0],
                                           bishop_square_tables[1], sq);
  }
  for (int p = 0; p < chess_state->white_pieces.dark_bishop_count; p++) {
    sq0x88_t sq = chess_state->white_pieces.dark_bishop_list[p];
    score += value_from_piece_square_table(mat_count, bishop_square_tables[0],
                                           bishop_square_tables[1],
                                           flipped_square(sq));
  }
  for (int p = 0; p < chess_state->black_pieces.dark_bishop_count; p++) {
    sq0x88_t sq = chess_state->black_pieces.dark_bishop_list[p];
    score -= value_from_piece_square_table(mat_count, bishop_square_tables[0],
                                           bishop_square_tables[1], sq);
  }
  for (int p = 0; p < chess_state->white_pieces.rook_count; p++) {
    sq0x88_t sq = chess_state->white_pieces.rook_list[p];
    score += value_from_piece_square_table(mat_count, rook_square_tables[0],
                                           rook_square_tables[1],
                                           flipped_square(sq));
  }
  for (int p = 0; p < chess_state->black_pieces.rook_count; p++) {
    sq0x88_t sq = chess_state->black_pieces.rook_list[p];
    score -= value_from_piece_square_table(mat_count, rook_square_tables[0],
                                           rook_square_tables[1], sq);
  }
  for (int p = 0; p < chess_state->white_pieces.queen_count; p++) {
    sq0x88_t sq = chess_state->white_pieces.queen_list[p];
    score += value_from_piece_square_table(mat_count, queen_square_tables[0],
                                           queen_square_tables[1],
                                           flipped_square(sq));
  }
  for (int p = 0; p < chess_state->black_pieces.queen_count; p++) {
    sq0x88_t sq = chess_state->black_pieces.queen_list[p];
    score -= value_from_piece_square_table(mat_count, queen_square_tables[0],
                                           queen_square_tables[1], sq);
  }

  return chess_state->black_to_move ? -score : score;
}

score_centipawn_t mobility_score(const chess_state_t* chess_state) {
  size_t white_mobility =
      psuedo_legal_move_count(chess_state, &chess_state->white_pieces);
  size_t black_mobility =
      psuedo_legal_move_count(chess_state, &chess_state->black_pieces);

  long mobility_difference = white_mobility - black_mobility;

  score_centipawn_t score = mobility_difference * 15;

  return chess_state->black_to_move ? -score : score;
}

score_centipawn_t pawn_weakness_score(const chess_state_t* chess_state) {
  int doubled_pawn_diff = 0;
  int isolated_pawn_diff = 0;
  int blocked_pawn_diff = 0;
  int white_pawns[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int black_pawns[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  for (int i = 0; i < chess_state->white_pieces.pawn_count; i++) {
    white_pawns[sq0x88_to_file07(chess_state->white_pieces.pawn_list[i])]++;
    if (piece(chess_state, chess_state->white_pieces.pawn_list[i] + 16) !=
        EMPTY) {
      blocked_pawn_diff++;
    }
  }
  for (int i = 0; i < chess_state->black_pieces.pawn_count; i++) {
    black_pawns[sq0x88_to_file07(chess_state->black_pieces.pawn_list[i])]++;
    if (piece(chess_state, chess_state->white_pieces.pawn_list[i] - 16) !=
        EMPTY) {
      blocked_pawn_diff--;
    }
  }
  for (int i = 0; i < 8; i++) {
    if (white_pawns[i] > 1) {
      doubled_pawn_diff++;
    }
    if (black_pawns[i] > 1) {
      doubled_pawn_diff--;
    }
    if (white_pawns[i] > 0 && (i == 0 || white_pawns[i - 1] == 0) &&
        (i == 7 || white_pawns[i + 1] == 0)) {
      isolated_pawn_diff++;
    }
    if (black_pawns[i] > 0 && (i == 0 || black_pawns[i - 1] == 0) &&
        (i == 7 || black_pawns[i + 1] == 0)) {
      isolated_pawn_diff--;
    }
  }

  int pawn_structure_diff =
      doubled_pawn_diff + blocked_pawn_diff + isolated_pawn_diff;
  score_centipawn_t pawn_weakness_score = -pawn_structure_diff * 25;
  return chess_state->black_to_move ? -pawn_weakness_score
                                    : pawn_weakness_score;
}