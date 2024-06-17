#ifndef FEN_H
#define FEN_H

#include "chess.h"

const static String start_board_FEN =
    string_literal("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

Board parse_FEN(String fen_string);

#endif