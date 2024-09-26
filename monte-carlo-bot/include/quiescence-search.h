#ifndef QUIESCENCE_SEARCH_H
#define QUIESCENCE_SEARCH_H

#include "bot-types.h"

score_centipawn_t check_extension_search(chess_state_t* chess_state,
                                         score_centipawn_t alpha,
                                         score_centipawn_t beta, int depth,
                                         int max_depth);

score_centipawn_t quiescence_search(chess_state_t* chess_state,
                                    score_centipawn_t alpha,
                                    score_centipawn_t beta, int depth,
                                    int max_depth);

#endif