#include "../include/iterative-deepening.h"

#include <stdio.h>
#include <time.h>

#include "../include/transposition-table.h"

table_t transposition_table;

move_t next_move(const chess_state_t* chess_state, move_t* moves,
                 size_t* move_count) {
  score_centipawn_t best_move_priority = MIN_SCORE;
  size_t best_move_index = *move_count;
  for (size_t i = 0; i < *move_count; i++) {
    if (get_priority(moves[i]) > best_move_priority) {
      if (is_legal(chess_state, moves[i])) {
        best_move_priority = get_priority(moves[i]);
        best_move_index = i;
      } else {
        moves[i--] = moves[--*move_count];  // swap erase illegal moves
      }
    }
  }

  if (*move_count == 0) {
    return null_move;
  }

  // swap erase best move once found so it cant be taken again
  move_t ret = moves[best_move_index];
  moves[best_move_index] = moves[--*move_count];
  return ret;
}

move_t remove_refutation(move_t* moves, size_t* move_count, move_t refutation) {
  for (size_t i = 0; i < *move_count; i++) {
    if (compare_moves(moves[i], refutation)) {
      moves[i] = moves[--*move_count];
      return refutation;
    }
  }
  return null_move;
}

struct timespec start_time;

size_t time_limit = 100;

int on_start() {
  new_table(&transposition_table, 8388608ull);
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  return 0;
}

int on_stop() {
  release_table(&transposition_table);
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
                                             move_t* best_moves,
                                             size_t* times) {
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
    // clear_table(&transposition_table);
    best_move = find_best_move_iteration(chess_state, moves, scores, move_count,
                                         max_depth);
  }

  on_stop();
  return best_move;
}

move_t select_best_move(move_t* moves, score_centipawn_t* scores,
                        size_t move_count, size_t index) {
  size_t best_index = index;
  score_centipawn_t best_score = scores[index];
  for (size_t i = index + 1; i < move_count; i++) {
    if (scores[i] > best_score) {
      best_score = scores[i];
      best_index = i;
    }
  }
  if (best_index == index) {
    return moves[index];
  }
  score_centipawn_t first_score = scores[index];
  move_t first_move = moves[index];
  scores[index] = scores[best_index];
  moves[index] = moves[best_index];
  scores[best_index] = first_score;
  moves[best_index] = first_move;
  return moves[index];
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

    if (stop()) {
      return best_move;
    }

    make_move(chess_state, move);
    score_centipawn_t score = -negamax(chess_state, MIN_SCORE, -best_score, 0, max_depth);
    unmake_move(chess_state);
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

score_centipawn_t negamax(chess_state_t* chess_state, score_centipawn_t alpha,
                          score_centipawn_t beta, int depth, int max_depth) {
  if (depth == max_depth) {
    return static_evaluation(chess_state);
  }

  if (is_draw(chess_state)) {
    return DRAW_SCORE_CENTIPAWNS;
  }

  node_t* node = lookup_table(&transposition_table, chess_state->zobrist);

  if (node && get_age(*node) == max_depth) {
    int node_type = get_type(*node);
    score_centipawn_t node_score = get_score(*node);
    if ((node_type == CUT_NODE && node_score >= beta) ||
        (node_type == ALL_NODE && node_score <= alpha) ||
        node_type == PV_NODE) {
      return node_score;
    }
  }

  move_t moves[256];
  size_t move_count;

  move_count = generate_moves(chess_state, moves);

  move_t first_move;

  if (node) {
    first_move = remove_refutation(moves, &move_count, get_refutation(*node));
    if (is_null_move(first_move)) {
      first_move = next_move(chess_state, moves, &move_count);
    }
  } else {
    first_move = next_move(chess_state, moves, &move_count);
  }

  if (is_null_move(first_move)) {
    return is_check(chess_state) ? CHECKMATE_SCORE_CENTIPAWNS
                                 : STALEMATE_SCORE_CENTIPAWNS;
  }

  score_centipawn_t best_score =
      evaluate_move(chess_state, first_move, alpha, beta, depth, max_depth);

  if (best_score >= beta) {
    update_table(&transposition_table, chess_state->zobrist, best_score,
                 CUT_NODE, first_move, max_depth, depth);
    return best_score;
  }

  if (best_score > alpha) {
    alpha = best_score;
  }
  move_t move;
  move_t best_move = first_move;
  while (!is_null_move(move = next_move(chess_state, moves, &move_count))) {
    if (stop()) {
      return best_score;
    }

    score_centipawn_t score =
        evaluate_move(chess_state, move, alpha, beta, depth, max_depth);

    if (score >= beta) {
      update_table(&transposition_table, chess_state->zobrist, best_score,
                   CUT_NODE, first_move, max_depth, depth);
      return score;
    }

    if (score > best_score) {
      best_score = score;
      best_move = move;
      if (score > alpha) {
        alpha = score;
      }
    }
  }

  uint32_t node_type = best_score <= alpha ? ALL_NODE : PV_NODE;

  update_table(&transposition_table, chess_state->zobrist, best_score,
               node_type, best_move, max_depth, depth);

  return best_score;
}
