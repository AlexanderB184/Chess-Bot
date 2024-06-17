#ifndef CHECK_H
#define CHECK_H

#include "board.h"
#include "move.h"
#include "move_validation.h"

int is_check(const Board* board);

int is_checkmate(const Board* board);

int is_stalemate(const Board* board);

int is_check_after_move(const Board* board, Move move);

int can_be_attacked(const Board* board, int from);

int can_be_attacked_by(const Board* board, Piece_Colour colour, int from);

#endif