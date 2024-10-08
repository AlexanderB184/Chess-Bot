#ifndef STATIC_EVAL_TABLES_H
#define STATIC_EVAL_TABLES_H

#include "bot-types.h"

static const score_centipawn_t pawn_base_value = 100;
static const score_centipawn_t knight_base_value = 350;
static const score_centipawn_t bishop_base_value = 350;
static const score_centipawn_t rook_base_value = 525;
static const score_centipawn_t queen_base_value = 1000;
static const score_centipawn_t king_base_value = 10000;

score_centipawn_t pawn_square_tables[2][64] = {
    {
        // mid game
        0,   0,   0,   0,   0,   0,   0,   0,    //
        130, 150, 150, 150, 150, 150, 150, 130,  //
        50,  60,  60,  70,  70,  60,  60,  50,   //
        0,   0,   20,  50,  50,  0,   0,   0,    //
        -20, -20, 10,  30,  30,  -20, -20, -20,  //
        -25, -40, 0,   15,  15,  -40, -40, -25,  //
        10,  20,  20,  -10, -10, 20,  20,  10,   //
        0,   0,   0,   0,   0,   0,   0,   0,    //
    },
    {
        // end game
        0,   0,   0,   0,   0,   0,   0,   0,    //
        235, 250, 250, 250, 250, 250, 250, 235,  //
        170, 180, 180, 180, 180, 180, 180, 170,  //
        110, 115, 115, 115, 115, 115, 115, 110,  //
        105, 105, 105, 105, 105, 105, 105, 105,  //
        90,  90,  90,  90,  90,  90,  90,  90,   //
        85,  85,  85,  85,  85,  85,  85,  85,   //
        0,   0,   0,   0,   0,   0,   0,   0,    //
    },
};

score_centipawn_t knight_square_tables[2][64] = {
    {
        -50, -25, -20, -20, -20, -20, -25, -50,  //
        -25, -15, -10, -10, -10, -10, -15, -25,  //
        -20, -10, 0,   0,   0,   0,   -10, -20,  //
        -20, -10, 0,   25,  25,  0,   -10, -20,  //
        -20, -10, 0,   25,  25,  0,   -10, -20,  //
        -20, -10, 0,   0,   0,   0,   -10, -20,  //
        -25, -15, -10, -10, -10, -10, -15, -25,  //
        -50, -25, -20, -20, -20, -20, -25, -50,  //
    },
    {
        -50, -25, -20, -20, -20, -20, -25, -50,  //
        -25, -15, -10, -10, -10, -10, -15, -25,  //
        -20, -10, 0,   0,   0,   0,   -10, -20,  //
        -20, -10, 0,   25,  25,  0,   -10, -20,  //
        -20, -10, 0,   25,  25,  0,   -10, -20,  //
        -20, -10, 0,   0,   0,   0,   -10, -20,  //
        -25, -15, -10, -10, -10, -10, -15, -25,  //
        -50, -25, -20, -20, -20, -20, -25, -50,  //
    }};

score_centipawn_t bishop_square_tables[2][64] = {
    {
        -50, -25, -20, -20, -20, -20, -25, -50,  //
        -25, -15, -10, -10, -10, -10, -15, -25,  //
        -20, -10, 0,   0,   0,   0,   -10, -20,  //
        -20, -10, 0,   25,  25,  0,   -10, -20,  //
        -20, -10, 0,   25,  25,  0,   -10, -20,  //
        -20, -10, 0,   0,   0,   0,   -10, -20,  //
        -25, -15, -10, -10, -10, -10, -15, -25,  //
        -50, -25, -20, -20, -20, -20, -25, -50,  //
    },
    {
        -50, -25, -20, -20, -20, -20, -25, -50,  //
        -25, 0,   10,  -10, -10, 10,  0,   -25,  //
        -20, 10,  0,   0,   0,   0,   10,  -20,  //
        -20, -10, 0,   25,  25,  0,   -10, -20,  //
        -20, -10, 0,   25,  25,  0,   -10, -20,  //
        -20, 10,  0,   0,   0,   0,   10,  -20,  //
        -25, 0,   10,  -10, -10, 10,  -5,  -25,  //
        -50, -25, -20, -20, -20, -20, -25, -50,  //
    }};

score_centipawn_t rook_square_tables[2][64] = {
    {
        -10, 0,  0,  0,  0,  0,  0,  -10,  //
        50,  50, 50, 50, 50, 50, 50, 50,   //
        0,   0,  0,  0,  0,  0,  0,  0,    //
        20,  0,  0,  0,  0,  0,  0,  20,   //
        20,  0,  0,  0,  0,  0,  0,  20,   //
        0,   0,  0,  0,  0,  0,  0,  0,    //
        0,   0,  0,  0,  0,  0,  0,  0,    //
        -10, 0,  0,  20, 20, 0,  0,  -10,  //
    },
    {
        -10, 0,  0,  0,  0,  0,  0,  -10,  //
        50,  50, 50, 50, 50, 50, 50, 50,   //
        0,   0,  10, 10, 10, 10, 0,  0,    //
        20,  0,  10, 10, 10, 10, 0,  20,   //
        20,  0,  10, 10, 10, 10, 0,  20,   //
        0,   0,  10, 10, 10, 10, 0,  0,    //
        0,   0,  0,  0,  0,  0,  0,  0,    //
        -10, 0,  0,  20, 20, 0,  0,  -10,  //
    }};

score_centipawn_t queen_square_tables[2][64] = {
    {
        0, 0, 0, 0, 0, 0, 0, 0,  //
        0, 0, 0, 0, 0, 0, 0, 0,  //
        0, 0, 0, 0, 0, 0, 0, 0,  //
        0, 0, 0, 0, 0, 0, 0, 0,  //
        0, 0, 0, 0, 0, 0, 0, 0,  //
        0, 0, 0, 0, 0, 0, 0, 0,  //
        0, 0, 0, 0, 0, 0, 0, 0,  //
        0, 0, 0, 0, 0, 0, 0, 0,  //
    },
    {
        -50, -20, -20, -20, -20, -20, -20, -50,  //
        -20, 0,   0,   10,  10,  0,   0,   -20,  //
        -20, 0,   15,  15,  15,  15,  0,   -20,  //
        -20, 10,  15,  25,  25,  15,  10,  -20,  //
        -20, 10,  15,  25,  25,  15,  10,  -20,  //
        -20, 0,   15,  15,  15,  15,  0,   -20,  //
        -20, 0,   0,   10,  10,  0,   0,   -20,  //
        -50, -20, -20, -20, -20, -20, -20, -50,  //
    },
};

score_centipawn_t king_square_tables[2][64] = {
    {
        -100, -100, -100, -100, -100, -100, -100, -100,  //
        -90,  -90,  -90,  -90,  -90,  -90,  -90,  -90,   //
        -80,  -80,  -80,  -80,  -80,  -80,  -80,  -80,   //
        -70,  -70,  -70,  -70,  -70,  -70,  -70,  -70,   //
        -60,  -60,  -60,  -60,  -60,  -60,  -60,  -60,   //
        -40,  -50,  -50,  -50,  -50,  -50,  -50,  -40,   //
        -10,  -10,  20,   20,   20,   20,   -10,  -10,   //
        0,    20,   10,   -10,  -10,  10,   25,   0,     //
    },
    {
        -150, -100, -100, -100, -100, -100, -100, -150,  //
        -100, -25,  -25,  -25,  -25,  -25,  -25,  -100,  //
        -100, -25,  0,    0,    0,    0,    -25,  -100,  //
        -100, -25,  0,    25,   25,   0,    -25,  -100,  //
        -100, -25,  0,    25,   25,   0,    -25,  -100,  //
        -100, -25,  0,    0,    0,    0,    -25,  -100,  //
        -100, -25,  -25,  -25,  -25,  -25,  -25,  -100,  //
        -150, -100, -100, -100, -100, -100, -100, -150,  //
    }};

#endif