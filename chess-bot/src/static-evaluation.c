#include "../include/static-evaluation.h"


score_centipawn_t static_evaluation(const chess_state_t* chess_state) {
  if (is_draw(chess_state)) {
    return 0;
  }
  score_centipawn_t score = material_score(chess_state);
  return score;
}

score_centipawn_t pawn_square_table[64] = {
    0,   0,   0,   0,   0,   0,   0,   0,
  230, 250, 250, 250, 250, 250, 250, 230,
  150, 160, 160, 170, 170, 160, 160, 150,
  100, 100, 120, 150, 150, 100, 100, 100,
   80,  80, 110, 130, 130,  80,  80,  80,
   75,  60, 100, 115, 115,  60,  60,  75,
  110, 120, 120,  90,  90, 120, 120, 110,
    0,   0,   0,   0,  0,   0,    0,   0,
};
score_centipawn_t knight_square_table[64] = {
  250, 275, 280, 280, 280, 280, 275, 250,
  275, 285, 290, 290, 290, 290, 285, 275,
  280, 290, 300, 300, 300, 300, 290, 280,
  280, 290, 300, 325, 325, 300, 290, 280,
  280, 290, 300, 325, 325, 300, 290, 280,
  280, 290, 300, 300, 300, 300, 290, 280,
  275, 285, 290, 290, 290, 290, 285, 275,
  250, 275, 280, 280, 280, 280, 275, 250,  
};
score_centipawn_t bishop_square_table[64] = {
  250, 275, 280, 280, 280, 280, 275, 250,
  275, 285, 290, 290, 290, 290, 285, 275,
  280, 290, 300, 300, 300, 300, 290, 280,
  280, 290, 300, 325, 325, 300, 290, 280,
  280, 290, 300, 325, 325, 300, 290, 280,
  280, 290, 300, 300, 300, 300, 290, 280,
  275, 285, 290, 290, 290, 290, 285, 275,
  250, 275, 280, 280, 280, 280, 275, 250,  
};
score_centipawn_t rook_square_table[64] = {
  490, 500, 500, 500, 500, 500, 500, 490,
  550, 550, 550, 550, 550, 550, 550, 550,
  500, 500, 500, 500, 500, 500, 500, 500,
  520, 500, 500, 500, 500, 500, 500, 520,
  520, 500, 500, 500, 500, 500, 500, 520,
  500, 500, 500, 500, 500, 500, 500, 500,
  500, 500, 500, 500, 500, 500, 500, 500,
  490, 500, 500, 520, 520, 500, 500, 490,
};
score_centipawn_t queen_square_table[64] = {
  900, 900, 900, 900, 900, 900, 900, 900,
  900, 900, 900, 900, 900, 900, 900, 900,
  900, 900, 900, 900, 900, 900, 900, 900,
  900, 900, 900, 900, 900, 900, 900, 900,
  900, 900, 900, 900, 900, 900, 900, 900,
  900, 900, 900, 900, 900, 900, 900, 900,
  900, 900, 900, 900, 900, 900, 900, 900,
  900, 900, 900, 900, 900, 900, 900, 900,
};
score_centipawn_t king_square_table[64] = {
  1900, 1900, 1900, 1900, 1900, 1900, 1900, 1900,
  1910, 1910, 1910, 1910, 1910, 1910, 1910, 1910,
  1920, 1920, 1920, 1920, 1920, 1920, 1920, 1920,
  1930, 1930, 1930, 1930, 1930, 1930, 1930, 1930,
  1940, 1940, 1940, 1940, 1940, 1940, 1940, 1940,
  1960, 1950, 1950, 1950, 1950, 1950, 1950, 1960,
  1990, 1990, 1970, 1970, 1970, 1970, 1990, 1990,
  2000, 2020, 2010, 1990, 1990, 2010, 2025, 2000,
};

sq0x88_t flipped_square(sq0x88_t square) {
  return rankfile_to_sq0x88(7 - sq0x88_to_rank07(square),
                            sq0x88_to_file07(square));
}

score_centipawn_t pawn_value(sq0x88_t square, piece_t colour) {
  if (colour & WHITE) square = flipped_square(square);
  return pawn_square_table[sq0x88_to_sq8x8(square)];
}

score_centipawn_t knight_value(sq0x88_t square, piece_t colour) {
  if (colour & WHITE) square = flipped_square(square);
  return knight_square_table[sq0x88_to_sq8x8(square)];
}

score_centipawn_t bishop_value(sq0x88_t square, piece_t colour) {
  if (colour & WHITE) square = flipped_square(square);
  return bishop_square_table[sq0x88_to_sq8x8(square)];
}

score_centipawn_t rook_value(sq0x88_t square, piece_t colour) {
  if (colour & WHITE) square = flipped_square(square);
  return rook_square_table[sq0x88_to_sq8x8(square)];
}

score_centipawn_t queen_value(sq0x88_t square, piece_t colour) {
  if (colour & WHITE) square = flipped_square(square);
  return queen_square_table[sq0x88_to_sq8x8(square)];
}

score_centipawn_t king_value(sq0x88_t square, piece_t colour) {
  if (colour & WHITE) square = flipped_square(square);
  return king_square_table[sq0x88_to_sq8x8(square)];
}

score_centipawn_t material_score(const chess_state_t* chess_state) {
  score_centipawn_t score = 0;

  {
    sq0x88_t sq = chess_state->white_pieces.king_square;
    score += king_value(sq, WHITE);
  }
  {
    sq0x88_t sq = chess_state->black_pieces.king_square;
    score -= king_value(sq, BLACK);
  }
  for (int p = 0; p < chess_state->white_pieces.pawn_count; p++) {
    sq0x88_t sq = chess_state->white_pieces.pawn_list[p];
    score += pawn_value(sq, WHITE);
  }
  for (int p = 0; p < chess_state->black_pieces.pawn_count; p++) {
    sq0x88_t sq = chess_state->black_pieces.pawn_list[p];
    score -= pawn_value(sq, BLACK);
  }
  for (int p = 0; p < chess_state->white_pieces.knight_count; p++) {
    sq0x88_t sq = chess_state->white_pieces.knight_list[p];
    score += knight_value(sq, WHITE);
  }
  for (int p = 0; p < chess_state->black_pieces.knight_count; p++) {
    sq0x88_t sq = chess_state->black_pieces.knight_list[p];
    score -= knight_value(sq, BLACK);
  }
  for (int p = 0; p < chess_state->white_pieces.light_bishop_count; p++) {
    sq0x88_t sq = chess_state->white_pieces.light_bishop_list[p];
    score += bishop_value(sq, WHITE);
  }
  for (int p = 0; p < chess_state->black_pieces.light_bishop_count; p++) {
    sq0x88_t sq = chess_state->black_pieces.light_bishop_list[p];
    score -= bishop_value(sq, BLACK);
  }
  for (int p = 0; p < chess_state->white_pieces.dark_bishop_count; p++) {
    sq0x88_t sq = chess_state->white_pieces.dark_bishop_list[p];
    score += bishop_value(sq, WHITE);
  }
  for (int p = 0; p < chess_state->black_pieces.dark_bishop_count; p++) {
    sq0x88_t sq = chess_state->black_pieces.dark_bishop_list[p];
    score -= bishop_value(sq, BLACK);
  }
  for (int p = 0; p < chess_state->white_pieces.rook_count; p++) {
    sq0x88_t sq = chess_state->white_pieces.rook_list[p];
    score += rook_value(sq, WHITE);
  }
  for (int p = 0; p < chess_state->black_pieces.rook_count; p++) {
    sq0x88_t sq = chess_state->black_pieces.rook_list[p];
    score -= rook_value(sq, BLACK);
  }
  for (int p = 0; p < chess_state->white_pieces.queen_count; p++) {
    sq0x88_t sq = chess_state->white_pieces.queen_list[p];
    score += queen_value(sq, WHITE);
  }
  for (int p = 0; p < chess_state->black_pieces.queen_count; p++) {
    sq0x88_t sq = chess_state->black_pieces.queen_list[p];
    score -= queen_value(sq, BLACK);
  }

  return chess_state->black_to_move ? -score : score;
}
