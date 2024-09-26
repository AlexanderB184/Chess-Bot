#ifndef BOT_TYPES_H
#define BOT_TYPES_H

#include <stdint.h>

#include "../../chess-lib/include/chess-lib-types.h"
#include "../../chess-lib/include/chess-lib.h"

#define CHECKMATE_SCORE_CENTIPAWNS (score_centipawn_t)(-16384)
#define STALEMATE_SCORE_CENTIPAWNS (score_centipawn_t)(0)
#define DRAW_SCORE_CENTIPAWNS (score_centipawn_t)(0)
#define MAX_SCORE (score_centipawn_t)(INT16_MAX)
#define MIN_SCORE (score_centipawn_t)(-INT16_MAX)

typedef short score_centipawn_t;


#endif