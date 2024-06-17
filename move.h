#ifndef MOVE_H
#define MOVE_H

#include "mylib/Funcs.h"

typedef struct Board Board;

typedef struct Move Move;

struct Move {
  int from, to;
  int promote_to;
};

void make_move(Board* board, Move move);
Board copy_make_move(const Board* board, Move move);

#endif