#ifndef STATIC_EVALUATION_H
#define STATIC_EVALUATION_H

#include "bot-types.h"

#include "../../chess-lib/include/chess-lib.h"

score_centipawn_t material_score(const chess_state_t*);

score_centipawn_t static_evaluation(const chess_state_t*);

#endif