#include "../include/eval.h"

// @todo piece square tables
score_cp_t eval(const chess_state_t* position) {
  
  if (is_draw(position)) {
    return DRAW_SCORE_CENTIPAWNS;
  }

  if (is_checkmate(position)) {
    return CHECKMATE_SCORE_CENTIPAWNS;
  }
  
  if (is_stalemate(position)) {
    return STALEMATE_SCORE_CENTIPAWNS;
  }

  score_cp_t score =
       + position->friendly_pieces->pawn_count         * PAWN_VALUE
       - position->enemy_pieces->pawn_count            * PAWN_VALUE
       + position->friendly_pieces->knight_count       * KNIGHT_VALUE
       - position->enemy_pieces->knight_count          * KNIGHT_VALUE
       + position->friendly_pieces->light_bishop_count * BISHOP_VALUE
       - position->enemy_pieces->light_bishop_count    * BISHOP_VALUE
       + position->friendly_pieces->dark_bishop_count  * BISHOP_VALUE
       - position->enemy_pieces->dark_bishop_count     * BISHOP_VALUE
       + position->friendly_pieces->rook_count         * ROOK_VALUE
       - position->enemy_pieces->rook_count            * ROOK_VALUE
       + position->friendly_pieces->queen_count        * QUEEN_VALUE
       - position->enemy_pieces->queen_count           * QUEEN_VALUE;

       score += material_score(position);

  return score;
}

sq0x88_t flipped_square(sq0x88_t sq) {
  return rankfile_to_sq0x88(7 - sq0x88_to_rank07(sq),
                            sq0x88_to_file07(sq));
}

score_cp_t material_score(const chess_state_t* chess_state) {
  score_cp_t score = 0;

  {
    sq0x88_t sq = chess_state->white_pieces.king_square;
    score += king_square_table[sq0x88_to_sq8x8(sq)];
  }
  {
    sq0x88_t sq = flipped_square(chess_state->black_pieces.king_square);
    score -= king_square_table[sq0x88_to_sq8x8(sq)];
  }
  for (int p = 0; p < chess_state->white_pieces.pawn_count; p++) {
    sq0x88_t sq = chess_state->white_pieces.pawn_list[p];
    score += pawn_square_table[sq0x88_to_sq8x8(sq)];
  }
  for (int p = 0; p < chess_state->black_pieces.pawn_count; p++) {
    sq0x88_t sq = flipped_square(chess_state->black_pieces.pawn_list[p]);
    score -= pawn_square_table[sq0x88_to_sq8x8(sq)];
  }
  for (int p = 0; p < chess_state->white_pieces.knight_count; p++) {
    sq0x88_t sq = chess_state->white_pieces.knight_list[p];
    score += knight_square_table[sq0x88_to_sq8x8(sq)];
  }
  for (int p = 0; p < chess_state->black_pieces.knight_count; p++) {
    sq0x88_t sq = flipped_square(chess_state->black_pieces.knight_list[p]);
    score -= knight_square_table[sq0x88_to_sq8x8(sq)];
  }
  for (int p = 0; p < chess_state->white_pieces.light_bishop_count; p++) {
    sq0x88_t sq = chess_state->white_pieces.light_bishop_list[p];
    score += bishop_square_table[sq0x88_to_sq8x8(sq)];
  }
  for (int p = 0; p < chess_state->black_pieces.light_bishop_count; p++) {
    sq0x88_t sq = flipped_square(chess_state->black_pieces.light_bishop_list[p]);
    score -= bishop_square_table[sq0x88_to_sq8x8(sq)];
  }
  for (int p = 0; p < chess_state->white_pieces.dark_bishop_count; p++) {
    sq0x88_t sq = chess_state->white_pieces.dark_bishop_list[p];
    score += bishop_square_table[sq0x88_to_sq8x8(sq)];
  }
  for (int p = 0; p < chess_state->black_pieces.dark_bishop_count; p++) {
    sq0x88_t sq = flipped_square(chess_state->black_pieces.dark_bishop_list[p]);
    score -= bishop_square_table[sq0x88_to_sq8x8(sq)];
  }
  for (int p = 0; p < chess_state->white_pieces.rook_count; p++) {
    sq0x88_t sq = chess_state->white_pieces.rook_list[p];
    score += rook_square_table[sq0x88_to_sq8x8(sq)];
  }
  for (int p = 0; p < chess_state->black_pieces.rook_count; p++) {
    sq0x88_t sq = flipped_square(chess_state->black_pieces.rook_list[p]);
    score -= rook_square_table[sq0x88_to_sq8x8(sq)];
  }
  for (int p = 0; p < chess_state->white_pieces.queen_count; p++) {
    sq0x88_t sq = chess_state->white_pieces.queen_list[p];
    score += queen_square_table[sq0x88_to_sq8x8(sq)];
  }
  for (int p = 0; p < chess_state->black_pieces.queen_count; p++) {
    sq0x88_t sq = flipped_square(chess_state->black_pieces.queen_list[p]);
    score -= queen_square_table[sq0x88_to_sq8x8(sq)];
  }

  return chess_state->black_to_move ? -score : score;
}


score_cp_t piece_value(sq0x88_t sq, piece_t p) {
  (void)sq;
  switch (p & PIECE_MASK) {
    case PAWN: return PAWN_VALUE;
    case KNIGHT: return KNIGHT_VALUE;
    case BISHOP: return BISHOP_VALUE;
    case ROOK: return ROOK_VALUE;
    case QUEEN: return QUEEN_VALUE;
    case KING: return KING_VALUE;
    default: return 0;
  }
}

int is_repetition(const chess_state_t* chess_state, int ply_of_root) {
  if (chess_state->ply_counter - chess_state->ply_of_last_irreversible_move <
      3) {
    return 0;
  }
  zobrist_t current_zobrist = chess_state->zobrist;
  int repetitions = 1;
  for (int i = chess_state->ply_counter - 2;
       i >= chess_state->ply_of_last_irreversible_move; i -= 2) {
    if (chess_state->ply_stack[i].zobrist == current_zobrist) {
      if (i >= ply_of_root) {
        return 1;
      }
      repetitions++;
    }
  }
  return repetitions >= 3;
}