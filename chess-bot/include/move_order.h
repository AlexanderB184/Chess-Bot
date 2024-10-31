#ifndef MOVE_ORDER_H
#define MOVE_ORDER_H

#include "../../chess-lib/include/chess-lib.h"

enum move_order_state {
  PRIORITY_PV_MOVE         = 0xA00,
  PRIORITY_WINNING_CAPTURE = 0x800,
  PRIORITY_NEUTRAL_CAPTURE = 0x600,
  PRIORITY_KILLER_MOVES    = 0x400,
  LOSING_CAPTURE           = 0x200,
  PRIORITY_QUIET_MOVE      = 0x000,
};

typedef struct move_list_t {
  move_t moves[256];
  size_t move_count;
} move_list_t;

void init_move_list(const chess_state_t* position, move_list_t* move_list, move_t hash_move);
move_t next_move(const chess_state_t* position, move_list_t* move_list);

#endif