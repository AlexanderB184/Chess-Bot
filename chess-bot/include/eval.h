#ifndef EVAL_H
#define EVAL_H

#include "bot.h"

#define PAWN_VALUE ((score_cp_t)100)
#define KNIGHT_VALUE ((score_cp_t)320)
#define BISHOP_VALUE ((score_cp_t)330)
#define ROOK_VALUE ((score_cp_t)500)
#define QUEEN_VALUE ((score_cp_t)900)
#define KING_VALUE ((score_cp_t)(9 * QUEEN_VALUE + 2 * ROOK_VALUE + 2 * BISHOP_VALUE + 2 * KNIGHT_VALUE + 1))

score_cp_t piece_value(sq0x88_t, piece_t piece);

/*
score_centipawn_t material_score(const chess_state_t*);

score_centipawn_t mobility_score(const chess_state_t*);

score_centipawn_t pawn_weakness_score(const chess_state_t*);

score_centipawn_t static_evaluation(const chess_state_t*);



score_centipawn_t board_value(const chess_state_t*, sq0x88_t);

score_centipawn_t pawn_value(sq0x88_t square, piece_t colour);

score_centipawn_t knight_value(sq0x88_t square, piece_t colour);

score_centipawn_t bishop_value(sq0x88_t square, piece_t colour);

score_centipawn_t rook_value(sq0x88_t square, piece_t colour);

score_centipawn_t queen_value(sq0x88_t square, piece_t colour);

score_centipawn_t king_value(sq0x88_t square, piece_t colour);
*/


score_cp_t eval(const chess_state_t* position);

#endif