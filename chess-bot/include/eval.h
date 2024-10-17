#ifndef EVAL_H
#define EVAL_H

#include "bot.h"

#define PAWN_VALUE ((score_cp_t)100)
#define KNIGHT_VALUE ((score_cp_t)320)
#define BISHOP_VALUE ((score_cp_t)330)
#define ROOK_VALUE ((score_cp_t)500)
#define QUEEN_VALUE ((score_cp_t)900)

score_cp_t eval(const chess_state_t* position);

#endif