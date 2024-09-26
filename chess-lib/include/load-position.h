#ifndef FEN_H
#define FEN_H

#include "chess-lib-types.h"

void load_start_position(chess_state_t* chess_state);
void copy_position(chess_state_t* chess_state,
                   const chess_state_t* state_to_copy);
const char* load_position(chess_state_t* chess_state, const char* FEN_string);
const char* save_fen();
void clear_position(chess_state_t* chess_state);

#endif