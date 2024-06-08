#ifndef BOARD_H
#define BOARD_H

typedef enum Piece_Type {
  EMPTY = 0,
  PAWN,
  KNIGHT,
  BISHOP,
  ROOK,
  QUEEN,
  KING
} Piece_Type;

typedef enum Piece_Colour { NONE = 0, WHITE = 1, BLACK = -1 } Piece_Colour;

typedef enum Castle_Rights {
  WHITE_QUEEN_SIDE = 0b1000,
  WHITE_KING_SIDE = 0b0100,
  BLACK_QUEEN_SIDE = 0b0010,
  BLACK_KING_SIDE = 0b0001,
  WHITE_BOTH_SIDES = WHITE_QUEEN_SIDE | WHITE_KING_SIDE,
  BLACK_BOTH_SIDES = BLACK_QUEEN_SIDE | BLACK_KING_SIDE,
  ALL_RIGHTS = WHITE_BOTH_SIDES | BLACK_BOTH_SIDES
} Castle_Rights;

typedef struct Piece {
  Piece_Colour colour;
  Piece_Type type;
} Piece;

typedef struct Board {
  Piece pieces[8][8];
  int enpassent_target_row, enpassent_target_col;
  int move_count, half_move_count;
  Piece_Colour turn;
  Castle_Rights rights;
} Board;

static const Board start_board = (const Board){
    .pieces =
        {
            {
                {WHITE, ROOK},
                {WHITE, KNIGHT},
                {WHITE, BISHOP},
                {WHITE, QUEEN},
                {WHITE, KING},
                {WHITE, BISHOP},
                {WHITE, KNIGHT},
                {WHITE, ROOK},
            },
            {
                {WHITE, PAWN},
                {WHITE, PAWN},
                {WHITE, PAWN},
                {WHITE, PAWN},
                {WHITE, PAWN},
                {WHITE, PAWN},
                {WHITE, PAWN},
                {WHITE, PAWN},
            },
            {
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
            },
            {
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
            },
            {
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
            },
            {
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
                {NONE, EMPTY},
            },
            {
                {BLACK, PAWN},
                {BLACK, PAWN},
                {BLACK, PAWN},
                {BLACK, PAWN},
                {BLACK, PAWN},
                {BLACK, PAWN},
                {BLACK, PAWN},
                {BLACK, PAWN},
            },
            {
                {BLACK, ROOK},
                {BLACK, KNIGHT},
                {BLACK, BISHOP},
                {BLACK, QUEEN},
                {BLACK, KING},
                {BLACK, BISHOP},
                {BLACK, KNIGHT},
                {BLACK, ROOK},
            },
        },
    .enpassent_target_row = 0,
    .enpassent_target_col = 0,
    .move_count = 0,
    .half_move_count = 0,
    .turn = WHITE,
    .rights = ALL_RIGHTS,
};

#endif