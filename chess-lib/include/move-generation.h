#ifndef MOVE_GENERATION_H
#define MOVE_GENERATION_H

#include "chess-lib-types.h"

size_t generate_legal_moves(const chess_state_t* chess_state, move_t* moves);

size_t generate_check_evasions(const chess_state_t* chess_state, move_t* moves);

size_t generate_psuedo_legal_moves(const chess_state_t* chess_state,
                                   move_t* moves);

size_t generate_moves(const chess_state_t* chess_state, move_t* moves);

size_t psuedo_legal_move_count(const chess_state_t* chess_state,
                               const piece_list_t* piece_list);

#endif