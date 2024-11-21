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

// @todo add incremental movegen so if a hash move exists we dont generate any
// moves until it has been tried, we then generate promotions, the captures,
// then try killer moves, then generate quiets
void init_move_list(const chess_state_t* position, move_list_t* move_list,
                    move_t hash_move, compact_move_t* killer_moves, int16_t* hh,
                    int16_t* bh) {
  *move_list = (move_list_t){
      .move_count = 0,
      .hash_move = hash_move,
      .killer_moves = killer_moves,
      .hh = hh,
      .bh = bh,
      .state = PRIORITY_PV_MOVE,
  };
}

move_t next_capture(const chess_state_t* position, move_list_t* move_list) {
  move_t selected_move;
  do {
    switch (move_list->state) {
      case PRIORITY_PV_MOVE:
        if (is_pseudo_legal(position, move_list->hash_move)) {
          selected_move = move_list->hash_move;
          move_list->state = PRIORITY_WINNING_CAPTURE;
          break;
        }
        move_list->state = PRIORITY_WINNING_CAPTURE;
      case PRIORITY_WINNING_CAPTURE:
        move_list->move_count = generate_moves(position, move_list->moves);
        move_list->state = PRIORITY_NEUTRAL_CAPTURE;
        for (int i = 0; i < move_list->move_count; i++) {
          uint16_t prio = PRIORITY_QUIET_MOVE;
          move_t move = move_list->moves[i];
          if (compare_moves(move, move_list->hash_move)) {
            prio = PRIORITY_QUIET_MOVE;
          } else if (is_promotion(move)) {
            prio = PRIORITY_WINNING_CAPTURE +
                   value_of(piece(position, move.to)) +
                   value_of(get_promotes_to(move));
          } else if (is_capture(move)) {
            // SEE reduces the number of nodes search substantially but still
            // increases run time potentially it will be worth re-enabling if
            // additional features cause the eval time to increase and therefor
            // the better branch efficiency will make up for the overhead
            /*int see = static_exchange_evaluation(position, move);
            if (see > 0) {
              prio = PRIORITY_WINNING_CAPTURE;
            } else if (see == 0) {
              prio = PRIORITY_NEUTRAL_CAPTURE;
            } else {
              prio = PRIORITY_LOSING_CAPTURE;
            }*/
            prio = PRIORITY_NEUTRAL_CAPTURE;
            prio += value_of(piece(position, move.to)) -
                    value_of(piece(position, move.from));
          } else {
            prio = PRIORITY_QUIET_MOVE;
          }
          move_list->moves[i] = set_priority(move, prio);
        }

      default: {
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
        if (!is_capture(selected_move) && !is_promotion(selected_move)) {
          return null_move;
        }
        move_list->moves[selected_idx] =
            move_list->moves[--move_list->move_count];
      } break;
    }

  } while (!is_legal(position, selected_move));
  return selected_move;
}

void generate_capture_priorities(const chess_state_t* position,
                                 move_list_t* move_list) {
  for (int i = 0; i < move_list->move_count; i++) {
    uint16_t prio = PRIORITY_QUIET_MOVE;
    move_t move = move_list->moves[i];
    if (compare_moves(move, move_list->hash_move)) {
      move_list->moves[i--] = move_list->moves[--move_list->move_count];
      continue;
    } else if (is_promotion(move)) {
      prio = PRIORITY_WINNING_CAPTURE + value_of(piece(position, move.to)) +
             value_of(get_promotes_to(move));
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
      prio += value_of(piece(position, move.to)) -
              value_of(piece(position, move.from));
      move_list->moves[i] = set_priority(move, prio);
    }
  }
}

void generate_quiet_priorities(const chess_state_t* position,
                               move_list_t* move_list) {
  for (int i = 0; i < move_list->move_count; i++) {
    uint16_t prio = PRIORITY_QUIET_MOVE;
    move_t move = move_list->moves[i];
    int is_killer_move = 0;
    for (int j = 0; j < MAX_KILLER_MOVES; j++) {
      if (compare_moves(move, move_list->moves[j])) {
        is_killer_move = 1;
        prio = PRIORITY_KILLER_MOVES;
      }
    }
    if (!is_killer_move) {
      if (get_butteryfly_board(move_list->bh, move) == 0) {
        prio = 0;
      } else {
        prio = get_butteryfly_board(move_list->hh, move) * 256 /
               get_butteryfly_board(move_list->bh, move);
      }
    }
    move_list->moves[i] = set_priority(move, prio);
  }
}

move_t next_move(const chess_state_t* position, move_list_t* move_list) {
  move_t selected_move;
  do {
    switch (move_list->state) {
      case PRIORITY_PV_MOVE:
        if (is_pseudo_legal(position, move_list->hash_move)) {
          selected_move = move_list->hash_move;
          move_list->state = PRIORITY_WINNING_CAPTURE;
          break;
        }
        move_list->state = PRIORITY_WINNING_CAPTURE;
      case PRIORITY_WINNING_CAPTURE:
        move_list->move_count = generate_moves(position, move_list->moves);
        move_list->state = PRIORITY_NEUTRAL_CAPTURE;
        generate_capture_priorities(position, move_list);

      case PRIORITY_NEUTRAL_CAPTURE: {
        if (move_list->move_count == 0) return null_move;
        int no_captures = 1;
        size_t selected_idx = 0;
        int max_priority = -1;
        for (size_t i = 0; i < move_list->move_count; i++) {
          if (!is_capture(move_list->moves[i]) &&
              !is_promotion(move_list->moves[i])) {
            continue;
          }
          no_captures = 0;
          int prio = get_priority(move_list->moves[i]);
          if (prio >= max_priority) {
            max_priority = prio;
            selected_idx = i;
          }
        }
        if (!no_captures) {
          selected_move = move_list->moves[selected_idx];
          move_list->moves[selected_idx] =
            move_list->moves[--move_list->move_count];
          break;
        }
        move_list->state = PRIORITY_LOSING_CAPTURE;
        generate_quiet_priorities(position, move_list);
      }
      default: {
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
        move_list->moves[selected_idx] =
            move_list->moves[--move_list->move_count];
      } break;
    }

  } while (!is_legal(position, selected_move));
  return selected_move;
}