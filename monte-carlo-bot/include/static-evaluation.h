#ifndef STATIC_EVALUATION_H
#define STATIC_EVALUATION_H

#include "../../chess-lib/include/chess-lib.h"
#include "bot-types.h"

score_centipawn_t material_score(const chess_state_t*);

score_centipawn_t mobility_score(const chess_state_t*);

score_centipawn_t pawn_weakness_score(const chess_state_t*);

score_centipawn_t static_evaluation(const chess_state_t*);

score_centipawn_t piece_value(sq0x88_t, piece_t piece);

score_centipawn_t board_value(const chess_state_t*, sq0x88_t);

score_centipawn_t pawn_value(sq0x88_t square, piece_t colour);

score_centipawn_t knight_value(sq0x88_t square, piece_t colour);

score_centipawn_t bishop_value(sq0x88_t square, piece_t colour);

score_centipawn_t rook_value(sq0x88_t square, piece_t colour);

score_centipawn_t queen_value(sq0x88_t square, piece_t colour);

score_centipawn_t king_value(sq0x88_t square, piece_t colour);

#endif