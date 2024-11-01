#include "../include/move_order.h"

#include "../include/killer_moves.h"
#include "../include/see.h"

void add_killer_move(compact_move_t* killer_moves, move_t move) {
  compact_move_t compressed_move = compress_move(move);
  if (compressed_move == killer_moves[0]) return;
  for (int i = MAX_KILLER_MOVES - 1; i > 0; i--) {
    killer_moves[i] = killer_moves[i - 1];
  }
  killer_moves[0] = compressed_move;
}

// @todo order promotions based on see and/or MVD/LVA
// @todo add relative history heuristic for ordering quiet moves
// @todo add incremental movegen so if a hash move exists we dont generate any
// moves until it has been tried, we then generate promotions, the captures,
// then try killer moves, then generate quiets
void init_move_list(const chess_state_t* position, move_list_t* move_list,
                    move_t hash_move, compact_move_t* killer_moves, int16_t* hh, int16_t* bh) {
  move_list->move_count = generate_moves(position, move_list->moves);
  for (int i = 0; i < move_list->move_count; i++) {
    uint16_t prio = PRIORITY_QUIET_MOVE;
    move_t move = move_list->moves[i];
    if (compare_moves(move, hash_move)) {
      prio = PRIORITY_PV_MOVE;
    } else if (is_promotion(move)) {
      prio = PRIORITY_WINNING_CAPTURE + value_of(piece(position, move.to)) + value_of(get_promotes_to(move));
    } else if (is_capture(move)) {
      // SEE reduces the number of nodes search substantially but still
      // increases run time potentially it will be worth re-enabling if
      // additional features cause the eval time to increase and therefor the
      // better branch efficiency will make up for the overhead
      /*int see = static_exchange_evaluation(position, move);
      if (see > 0) {
        prio = PRIORITY_WINNING_CAPTURE;
      } else if (see == 0) {
        prio = PRIORITY_NEUTRAL_CAPTURE;
      } else {
        prio = PRIORITY_LOSING_CAPTURE;
      }*/
      prio = PRIORITY_NEUTRAL_CAPTURE;
      prio += value_of(piece(position, move.to)) - value_of(piece(position, move.from));
    } else {
      compact_move_t compressed_move = compress_move(move);
      for (int i = 0; i < MAX_KILLER_MOVES; i++) {
        if (killer_moves[i] == compressed_move) {
          prio = PRIORITY_KILLER_MOVES;
          break;
        }
      }
      if (get_butteryfly_board(bh, move) != 0) {
        prio = (get_butteryfly_board(hh, move) * 0x200) / get_butteryfly_board(bh, move);
      }
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