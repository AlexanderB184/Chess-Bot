#ifndef MOVE_H
#define MOVE_H

#include "mylib/Funcs.h"

typedef struct Board Board;

typedef struct Move Move;

struct Move {
  int piece_start_row, piece_start_col;
  int piece_end_row, piece_end_col;
  int promote_to;
};

void make_move(Board* board, Move move);

#endif