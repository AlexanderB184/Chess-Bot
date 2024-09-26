#include "bot-types.h"

enum {
  PRIORITY_MAX = 0xFFF,
  PRIORITY_WINNING_CAPTURE_SEQUENCE = 0x800,
  PRIORITY_NEUTRAL_CAPTURE_SEQUENCE = 0x600,
  PRIORITY_KILLER_MOVE = 0x400,
  PRIORITY_LOSING_CAPTURE_SEQUENCE = 0x200,
  PRIORITY_QUIET_MOVE = 0x000,
};

move_t next_move(const chess_state_t* chess_state, move_t* move_list,
                 size_t* move_count);

move_t remove_move(const chess_state_t* chess_state, move_t* move_list,
                   size_t* move_count, move_t move);

move_t select_best_move(move_t* move_list, score_centipawn_t* scores,
                        size_t move_count, size_t index);

void generate_priorities(const chess_state_t* chess_state, move_t* move_list,
                         size_t move_count, move_t refutation);