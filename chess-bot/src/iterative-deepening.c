#include "../include/iterative-deepening.h"

#include <stdio.h>
#include <time.h>

#include "../include/butterfly-board.h"
#include "../include/debug.h"
#include "../include/move-order.h"
#include "../include/quiescence-search.h"
#include "../include/super-simple-transposition-table.h"

table_t transpo_table;

uint8_t history_heuristic[64 * 64];
uint8_t butterfly_heuristic[64 * 64];

debug_info_t debug_info;

struct timespec start_time;

size_t time_limit = 100;

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 0
#endif

int on_start() {
  DEBUG_CLEAR();
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  make_table(&transpo_table, 10000, TT_ALWAYS_REPLACE);
  clear_butterfly_board(history_heuristic);
  clear_butterfly_board(butterfly_heuristic);
  return 0;
}

int on_stop() {
  // DEBUG_SET_UNIQUE_NODES_SEARCHED(transpo_table.item_count)
  release_table(&transpo_table);

  struct timespec current_time;
  clock_gettime(CLOCK_MONOTONIC, &current_time);
  size_t elapsed_time = (current_time.tv_sec - start_time.tv_sec) * 1000;
  elapsed_time += (current_time.tv_nsec - start_time.tv_nsec) / 1000000;
  DEBUG_SET_TIME_ELAPSED((current_time.tv_sec - start_time.tv_sec) *
                             1000000000 +
                         (current_time.tv_nsec - start_time.tv_nsec))
  return 0;
}

int stop() {
  struct timespec current_time;
  clock_gettime(CLOCK_MONOTONIC, &current_time);

  size_t elapsed_time = (current_time.tv_sec - start_time.tv_sec) * 1000;
  elapsed_time += (current_time.tv_nsec - start_time.tv_nsec) / 1000000;

  return elapsed_time >= time_limit;
}

int find_best_move_iterative_deepening_test(chess_state_t* chess_state,
                                            move_t* best_moves, size_t* times) {
  on_start();

  move_t moves[256];
  size_t move_count = generate_legal_moves(chess_state, moves);
  score_centipawn_t scores[256] = {0};

  int max_depth;
  for (max_depth = 0; !stop(); max_depth++) {
    // clear_table(&transposition_table);
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    best_moves[max_depth] = find_best_move_iteration(chess_state, moves, scores,
                                                     move_count, max_depth);
    // clear_table();
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    size_t elapsed_time =
        (end_time.tv_sec - start_time.tv_sec) * 1000 * 1000 * 1000;
    elapsed_time += (end_time.tv_nsec - start_time.tv_nsec);
    printf("depth: %d\n", max_depth);
    times[max_depth] = elapsed_time;
  }
  // release_table();

  on_stop();
  return max_depth;
}

move_t find_best_move(chess_state_t* chess_state) {
  on_start();

  move_t best_move;

  move_t moves[256];
  size_t move_count = generate_legal_moves(chess_state, moves);
  score_centipawn_t scores[256] = {0};

  for (int max_depth = 0; !stop(); max_depth++) {
    best_move = find_best_move_iteration(chess_state, moves, scores, move_count,
                                         max_depth);
  }

  on_stop();
  return best_move;
}

move_t find_best_move_iteration(chess_state_t* chess_state, move_t* moves,
                                score_centipawn_t* scores, size_t move_count,
                                int max_depth) {
  size_t i = 0;
  move_t best_move = select_best_move(moves, scores, move_count, i);
  make_move(chess_state, best_move);
  score_centipawn_t best_score =
      -negamax(chess_state, MIN_SCORE, MAX_SCORE, 0, max_depth);
  unmake_move(chess_state);
  scores[0] = best_score;

  for (i = 1; i < move_count; i++) {
    move_t move = select_best_move(moves, scores, move_count, i);

    make_move(chess_state, move);
    score_centipawn_t score =
        -negamax(chess_state, MIN_SCORE, -best_score, 0, max_depth);
    unmake_move(chess_state);

    if (stop()) {
      DEBUG_SET_MOVES_EVALUATED_FINAL_ITER(i);
      DEBUG_SET_POSSIBLE_MOVES(move_count);
      DEBUG_SET_MAX_DEPTH_REACHED(max_depth);

      return best_move;
    }

    scores[i] = score;
    if (score > best_score) {
      best_score = score;
      best_move = move;
    }
  }

  return best_move;
}

score_centipawn_t evaluate_move(chess_state_t* chess_state, move_t move,
                                score_centipawn_t alpha, score_centipawn_t beta,
                                int depth, int max_depth) {
  make_move(chess_state, move);
  score_centipawn_t score =
      -negamax(chess_state, -beta, -alpha, depth + 1, max_depth);
  unmake_move(chess_state);
  return score;
}

int transposition_cutoff_policy(node_t* node, score_centipawn_t alpha,
                                score_centipawn_t beta, int depth,
                                int max_depth) {
  int node_type = node_get_type(node);
  score_centipawn_t node_score = node_get_score(node);

  return (node_get_depth_searched(node) >= max_depth - depth) &&
         ((node_type == TTNODE_LOWER && node_score >= beta) ||
          (node_type == TTNODE_UPPER && node_score <= alpha) ||
          node_type == TTNODE_EXACT);
}

size_t generate_moves_with_priorities(const chess_state_t* chess_state,
                                      move_t* moves, move_t hash_move) {
  size_t move_count = generate_moves(chess_state, moves);
  generate_priorities(chess_state, moves, move_count, hash_move);
  return move_count;
}

int is_onefold_repeition_since_root(const chess_state_t* chess_state,
                                    int depth) {
  for (int i = 1; i < depth; i++) {
    if (chess_state->ply_stack[chess_state->ply_counter - i - 1].zobrist ==
        chess_state->zobrist) {
      return 1;
    }
  }
  return 0;
}

score_centipawn_t negamax(chess_state_t* chess_state, score_centipawn_t alpha,
                          score_centipawn_t beta, int depth, int max_depth) {
  DEBUG_INC_TOTAL_NODES_SEARCHED();
  if (depth == max_depth) {
    return quiescence_search(chess_state, alpha, beta, depth, max_depth);
  }

  if (is_draw(chess_state)) {
    return DRAW_SCORE_CENTIPAWNS;
  }

  // repetitions since root are assigned to be drawn
  // if (is_onefold_repeition_since_root(chess_state, depth)) {
  //  return DRAW_SCORE_CENTIPAWNS;
  //}

  move_t hash_move = null_move;

  node_t* node = get_node(&transpo_table, chess_state->zobrist);

  if (node != NULL) {
    if (transposition_cutoff_policy(node, alpha, beta, depth, max_depth)) {
      DEBUG_INC_TRANSPO_CUTOFFS();
      return node_get_score(node);
    }
    hash_move = node_get_hash_move(node);
  }

  move_t moves[256];
  size_t move_count;

  move_count = generate_moves_with_priorities(chess_state, moves, hash_move);

  move_t move = next_move(chess_state, moves, &move_count);

  if (is_null_move(move)) {
    return is_check(chess_state) ? CHECKMATE_SCORE_CENTIPAWNS
                                 : STALEMATE_SCORE_CENTIPAWNS;
  }

  score_centipawn_t best_score = MIN_SCORE;
  move_t best_move = move;

  do {
    score_centipawn_t score =
        evaluate_move(chess_state, move, alpha, beta, depth, max_depth);

    if (stop()) {
      return best_score;
    }

    if (!is_capture(move)) {
      inc_butterfly_board(butterfly_heuristic, move);
    }

    if (score >= beta) {
      DEBUG_INC_BETA_CUTOFFS();
      set_node(&transpo_table, chess_state->zobrist, move, score, TTNODE_LOWER,
               depth, max_depth);
      if (!is_capture(move)) {
        inc_butterfly_board(history_heuristic, move);
      }
      return score;
    }

    if (score > best_score) {
      best_score = score;
      best_move = move;

      if (score > alpha) {
        alpha = score;
      }
    }

  } while (!is_null_move(move = next_move(chess_state, moves, &move_count)));

  int node_type = best_score <= alpha ? TTNODE_UPPER : TTNODE_EXACT;

  set_node(&transpo_table, chess_state->zobrist, best_move, best_score,
           node_type, depth, max_depth);

  return best_score;
}
