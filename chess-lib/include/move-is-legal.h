#ifndef IS_LEGAL_H
#define IS_LEGAL_H

#include "chess-lib.h"

// assumes the position is not in check.
// returns true if the move doesn't move an absolutely pinned piece
int is_legal(const chess_state_t* chess_state, move_t move);

#endif