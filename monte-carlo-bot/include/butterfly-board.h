#ifndef BUTTERFLY_BOARDS_H
#define BUTTERFLY_BOARDS_H

#include "bot-types.h"

typedef uint8_t butterfly_board_t[64 * 64];

extern butterfly_board_t history_heuristic;
extern butterfly_board_t butterfly_heuristic;

butterfly_index_t get_butterfly_index(move_t move);

void inc_butterfly_board(butterfly_board_t* butterfly_board, move_t move);

void clear_butterfly_board(butterfly_board_t* butterfly_board);

uint8_t get_butterfly_board(butterfly_board_t* butterfly_board, move_t move);

void set_butterfly_board(butterfly_board_t* butterfly_board, move_t move, uint8_t value);

#endif