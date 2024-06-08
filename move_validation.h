#ifndef MOVE_VALID_H
#define MOVE_VALID_H

#include "attack.h"
#include "board.h"
#include "move.h"

int is_legal_move(const Board*, Move);
int is_psuedo_legal_move(const Board*, Move);

#endif