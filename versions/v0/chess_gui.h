#ifndef CHESS_GUI_H
#define CHESS_GUI_H

#include <stdio.h>

#include "mylib/String.h"

#include "board.h"
#include "move.h"

void display_board(FILE* out, const Board* board, Piece_Colour turn);

Move get_move(FILE* in, const Board* board);

Move get_legal_move(FILE* in, const Board* board);

Move parse_move(const Board* board, String command);

#endif