#ifndef ITERATIVE_DEEPENING_H
#define ITERATIVE_DEEPENING_H

#include "../../chess-lib/include/chess-lib.h"
#include "bot-types.h"

move_t find_best_move(chess_state_t* chess_state);

move_t find_best_move_iteration(chess_state_t* chess_state, move_t* moves, score_centipawn_t* scores, size_t move_count, int max_depth);

score_centipawn_t negamax(chess_state_t* chess_state, score_centipawn_t alpha, score_centipawn_t beta,
                          int depth, int max_depth);

score_centipawn_t static_evaluation(const chess_state_t* chess_state);

#endif