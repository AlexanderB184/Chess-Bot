#ifndef STATIC_EVAL_H
#define STATIC_EVAL_H

#include "../chess.h"

double static_piece_value(Piece_Type type);
int is_endgame(const Board* board);
int is_promotion(const Board* board, Move move);
int is_pawn_promoting(const Board* board);
int is_capture(const Board* board, Move move);

double static_evaluation(const Board* board);

#endif