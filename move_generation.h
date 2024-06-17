#ifndef MOVE_GEN_H
#define MOVE_GEN_H

#include "mylib/Arena.h"
#include "mylib/Slice.h"
#include "board.h"
#include "move.h"

Slice_proto(Move);

Slice(Move) generate_psuedo_legal_moves(Slice(Move) buffer, const Board* board);

Slice(Move) generate_legal_moves(Slice(Move) buffer, const Board* board);

#endif