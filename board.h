#ifndef BOARD_H
#define BOARD_H

typedef enum Piece_Type {
  EMPTY = 0,
  PAWN,
  KNIGHT,
  BISHOP,
  ROOK,
  QUEEN,
  KING,
  INVALID,
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
  Piece pieces[64];
  int enpassent;
  int move_count, half_move_count;
  Piece_Colour turn;
  Castle_Rights rights;
} Board;

Piece board_get_piece(const Board* board, int flat_index);
void board_set_piece(Board* board, int flat_index, Piece);
#define index_of(ROW, COL) ((ROW) * 8 + (COL))
#define rank_of(idx) ((idx) / 8)
#define file_of(idx) ((idx) % 8)
#define rank_diff(FROM, TO) rank_of(TO) - rank_of(FROM)
#define file_diff(FROM, TO) file_of(TO) - file_of(FROM)
/*
enum Letter_Box_Piece_Values {
  LB_EMPTY = 0,
  LB_P = 1,
  LB_N = 2,
  LB_B = 3,
  LB_R = 4,
  LB_Q = 5,
  LB_K = 6,
  LB_p = -LB_P,
  LB_n = -LB_N,
  LB_b = -LB_B,
  LB_r = -LB_R,
  LB_q = -LB_Q,
  LB_k = LB_K
};

typedef struct Compressed_Piece {
  uint8_t letter_box_piece_value;
} Compressed_Piece;

typedef struct Compressed_Board {
  Compressed_Piece pieces[64];  // 64 bytes
  int move_count;               // 4 bytes
  union {
    struct {
      uint8_t half_move_count;
      uint8_t enpassent_target;
      uint8_t castle_rights;
      uint8_t turn;
    };
    uint32_t bitfield;
  } bitfield;

} Compressed_Board;*/

static const Board start_board = (const Board){
    .pieces =
        {

            {WHITE, ROOK}, {WHITE, KNIGHT}, {WHITE, BISHOP}, {WHITE, QUEEN},
            {WHITE, KING}, {WHITE, BISHOP}, {WHITE, KNIGHT}, {WHITE, ROOK},

            {WHITE, PAWN}, {WHITE, PAWN},   {WHITE, PAWN},   {WHITE, PAWN},
            {WHITE, PAWN}, {WHITE, PAWN},   {WHITE, PAWN},   {WHITE, PAWN},

            {NONE, EMPTY}, {NONE, EMPTY},   {NONE, EMPTY},   {NONE, EMPTY},
            {NONE, EMPTY}, {NONE, EMPTY},   {NONE, EMPTY},   {NONE, EMPTY},

            {NONE, EMPTY}, {NONE, EMPTY},   {NONE, EMPTY},   {NONE, EMPTY},
            {NONE, EMPTY}, {NONE, EMPTY},   {NONE, EMPTY},   {NONE, EMPTY},

            {NONE, EMPTY}, {NONE, EMPTY},   {NONE, EMPTY},   {NONE, EMPTY},
            {NONE, EMPTY}, {NONE, EMPTY},   {NONE, EMPTY},   {NONE, EMPTY},

            {NONE, EMPTY}, {NONE, EMPTY},   {NONE, EMPTY},   {NONE, EMPTY},
            {NONE, EMPTY}, {NONE, EMPTY},   {NONE, EMPTY},   {NONE, EMPTY},

            {BLACK, PAWN}, {BLACK, PAWN},   {BLACK, PAWN},   {BLACK, PAWN},
            {BLACK, PAWN}, {BLACK, PAWN},   {BLACK, PAWN},   {BLACK, PAWN},

            {BLACK, ROOK}, {BLACK, KNIGHT}, {BLACK, BISHOP}, {BLACK, QUEEN},
            {BLACK, KING}, {BLACK, BISHOP}, {BLACK, KNIGHT}, {BLACK, ROOK},

        },
    .enpassent = -1,
    .move_count = 0,
    .half_move_count = 0,
    .turn = WHITE,
    .rights = ALL_RIGHTS,
};

#endif