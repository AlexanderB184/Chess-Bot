#include "../include/move_order.h"

void init_move_list(const chess_state_t* position, move_list_t* move_list, move_t hash_move) {
  move_list->move_count = generate_moves(position, move_list->moves);
  for (int i = 0; i < move_list->move_count; i++) {
    uint16_t prio = PRIORITY_QUIET_MOVE;
    move_t move = move_list->moves[i];
    if (compare_moves(move, hash_move)) {
      prio = PRIORITY_PV_MOVE;
    } else if (is_capture(move) || is_promotion(move)) {
      prio = PRIORITY_WINNING_CAPTURE;
    } else {
      prio = PRIORITY_QUIET_MOVE;
    }
    move_list->moves[i] = set_priority(move, prio);
  }
}

move_t next_move(const chess_state_t* position, move_list_t* move_list) {
  move_t selected_move;
  do {
    if (move_list->move_count == 0) return null_move;
    size_t selected_idx = 0;
    int max_priority = get_priority(move_list->moves[0]);
    for (size_t i = 1; i < move_list->move_count; i++) {
      int prio = get_priority(move_list->moves[i]);
      if (prio > max_priority) {
        max_priority = prio;
        selected_idx = i;
      }
    }
    selected_move = move_list->moves[selected_idx];
    move_list->moves[selected_idx] = move_list->moves[--move_list->move_count];
  } while (!is_legal(position, selected_move));
  return selected_move;
}