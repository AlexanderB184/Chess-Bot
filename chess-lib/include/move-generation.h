#ifndef MOVE_GENERATION_H
#define MOVE_GENERATION_H

#include "chess-lib-types.h"

size_t generate_legal_moves(const chess_state_t* chess_state, move_t* moves);

size_t generate_check_evasions(const chess_state_t* chess_state, move_t* moves);

size_t generate_psuedo_legal_moves(const chess_state_t* chess_state, move_t* moves);

size_t generate_moves(const chess_state_t* chess_state, move_t* moves);

#endif