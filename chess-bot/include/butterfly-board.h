#ifndef BUTTERFLY_BOARDS_H
#define BUTTERFLY_BOARDS_H

#include "bot-types.h"

extern uint8_t history_heuristic[64 * 64];
extern uint8_t butterfly_heuristic[64 * 64];

butterfly_index_t get_butterfly_index(move_t move);

void inc_butterfly_board(uint8_t* butterfly_board, move_t move);

void clear_butterfly_board(uint8_t* butterfly_board);

uint8_t get_butterfly_board(uint8_t* butterfly_board, move_t move);

void set_butterfly_board(uint8_t* butterfly_board, move_t move, uint8_t value);

#endif