#ifndef MOVE_ORDER_H
#define MOVE_ORDER_H

#include "../../chess-lib/include/chess-lib.h"

#define BUTTERFLY_BOARD_SIZE (64 * 64)

#include <string.h>

static inline int butterfly_index(move_t move) {
  int from = sq0x88_to_sq8x8(move.from);
  int to = sq0x88_to_sq8x8(move.to);
  return (to<<6)+from;
}

static inline void inc_butteryfly_board(int16_t* board, move_t move) {
  board[butterfly_index(move)]++;
}

static inline int16_t get_butteryfly_board(int16_t* board, move_t move) {
  return board[butterfly_index(move)];
}

static inline void reset_butterfly_board(int16_t* board) {
  memset(board, 0, sizeof(int16_t) * BUTTERFLY_BOARD_SIZE);
}

enum move_order_state {
  PRIORITY_PV_MOVE         = 0xA00,
  PRIORITY_WINNING_CAPTURE = 0x800,
  PRIORITY_NEUTRAL_CAPTURE = 0x600,
  PRIORITY_KILLER_MOVES    = 0x400,
  PRIORITY_LOSING_CAPTURE  = 0x200,
  PRIORITY_QUIET_MOVE      = 0x000,
};

typedef struct move_list_t {
  move_t moves[256];
  size_t move_count;
  move_t hash_move;
  compact_move_t* killer_moves;
  int16_t* bh, *hh;
  enum move_order_state state;
} move_list_t;

void init_move_list(const chess_state_t* position, move_list_t* move_list, move_t hash_move, compact_move_t* killer_moves, int16_t*,int16_t*);
move_t next_move(const chess_state_t* position, move_list_t* move_list);
move_t next_capture(const chess_state_t* position, move_list_t* move_list);

#endif