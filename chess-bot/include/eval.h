#ifndef EVAL_H
#define EVAL_H

#include "bot.h"

#define PAWN_VALUE ((score_cp_t)100)
#define KNIGHT_VALUE ((score_cp_t)320)
#define BISHOP_VALUE ((score_cp_t)330)
#define ROOK_VALUE ((score_cp_t)500)
#define QUEEN_VALUE ((score_cp_t)900)
#define KING_VALUE ((score_cp_t)(9 * QUEEN_VALUE + 2 * ROOK_VALUE + 2 * BISHOP_VALUE + 2 * KNIGHT_VALUE + 1))

score_cp_t piece_value(sq0x88_t, piece_t piece);

score_cp_t material_score(const chess_state_t*);
/*

score_centipawn_t mobility_score(const chess_state_t*);

score_centipawn_t pawn_weakness_score(const chess_state_t*);

score_centipawn_t static_evaluation(const chess_state_t*);



score_centipawn_t board_value(const chess_state_t*, sq0x88_t);

score_centipawn_t pawn_value(sq0x88_t square, piece_t colour);

score_centipawn_t knight_value(sq0x88_t square, piece_t colour);

score_centipawn_t bishop_value(sq0x88_t square, piece_t colour);

score_centipawn_t rook_value(sq0x88_t square, piece_t colour);

score_centipawn_t queen_value(sq0x88_t square, piece_t colour);

score_centipawn_t king_value(sq0x88_t square, piece_t colour);
*/


score_cp_t eval(const chess_state_t* position);

int is_repetition(const chess_state_t* position, int ply_of_root);

const static score_cp_t king_square_table[64] = {
     20,  50,   0, -10, -25,  50,  60,  20,
     10,  20, -50, -75, -50,  20,  20,  10,
     10, -20, -75,-100,-100, -50, -50,  10,
    -20, -50,-110,-110,-110,-110,-110, -20,
   -125,-125,-125,-125,-125,-125,-125,-125,
   -150,-150,-150,-150,-150,-150,-150,-150, 
   -175,-175,-175,-175,-175,-175,-175,-175,
   -200,-200,-200,-200,-200,-200,-200,-200,
};
const static score_cp_t queen_square_table[64] = {
      0,   5,  10,  10,   5,   5,   5,   0,
      0,   5,  20,  15,  10,   5,  10,   0,
      0,  20,   0,   0,   0,  10,   0,   0,
     15,   0,   0,   0,   0,   0,   5,   0,
      0,   0,   0,   0,   0,   0,   0,   5,
      0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0,
    -10, -10, -10, -10, -10, -10, -10, -10,
};
const static score_cp_t rook_square_table[64] = {
      0,  10,  20,  30,  30,  20,  10,   0,
     10,   0,   0,  20,  20,   0,   0,  10,
     20,   0,   0,   0,   0,   0,   0,  20,
     30,   0,   0,   0,   0,   0,   0,  30,
     30,   0,   0,   0,   0,   0,   0,  30,
     20,   0,   0,   0,   0,   0,   0,  20, 
     50,  50,  50,  50,  50,  50,  50,  50,
      0,  10,  20,  30,  30,  20,  10,   0,
};
const static score_cp_t bishop_square_table[64] = {
     10,  -5,  -5,  -5,  -5,  -5,  -5,  10,
      5,  20,  10,  20,  20,  10,  20,   0,
     10,   0,   5,  20,  20,   5,   0,  10,
      0,   0,  20,  10,  10,  20,   0,   0,
      0,   0,   0,  10,  10,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0,
    -10,   0,   0,   0,   0,   0,   0, -10,
};
const static score_cp_t knight_square_table[64] = { // 10 for each plmove
   -100, -10, -10, -10, -10, -10, -10,-100,
    -40,   0,  10,  10,  10,  10,   0, -40,
    -20,   0,  20,  20,  20,  20,   0, -20,
    -10,   0,  20,  40,  40,  20,   0, -10,
    -10,   0,  20,  40,  40,  20,   0, -10,
    -20,   0,  20,  20,  20,  20,   0, -20, 
    -60, -40, -10, -10, -10, -10, -40, -60,
   -100, -80, -50, -50, -50, -50, -80,-100,
};

const static score_cp_t pawn_square_table[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,
     50,  50, -10, -10, -10,  15,  35,  50,
     10,  10,   5,   5,   5,  10,  10,  10,
     20, -10,  20,  20,  20,   5, -10,  20,
     30,  10,  30,  30,  30,  15,  10,  30,
     75,  75,  75,  75,  75,  75,  75,  75, 
    200, 200, 200, 200, 200, 200, 200, 200,
      0,   0,   0,   0,   0,   0,   0,   0,
    };

#endif