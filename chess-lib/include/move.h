#ifndef MOVE_H
#define MOVE_H

#include "chess-lib-types.h"

// updates the games state
void make_move(chess_state_t* chess_state,move_t move);

// restores the position to the previous state
void unmake_move(chess_state_t* chess_state);

// move constructor
move_t move(sq8x8_t from, sq8x8_t to, uint16_t flags, piece_t moved_piece, piece_t captured_piece);

move_t unordered_move(sq8x8_t from, sq8x8_t to, uint16_t flags);


#endif