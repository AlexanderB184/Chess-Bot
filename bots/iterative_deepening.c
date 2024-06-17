#include <stdlib.h>
#include <time.h>

#include "alpha_beta.h"
#include "debug.h"
#include "id.h"
#include "quiescence.h"
#include "transposition_table.h"

dbg_header();

typedef struct Move_Eval_Pair {
  double eval;
  Move move;
} Move_Eval_Pair;

double alpha_beta_id(const Board* board, double alpha, double beta,
                     size_t max_depth, size_t time_limit);

struct timespec start_time;

void init_clock() { clock_gettime(CLOCK_MONOTONIC, &start_time); }

size_t get_time_passed() {
  struct timespec current_time;
  clock_gettime(CLOCK_MONOTONIC, &current_time);

  size_t elapsed_time = (current_time.tv_sec - start_time.tv_sec) * 1000;
  elapsed_time += (current_time.tv_nsec - start_time.tv_nsec) / 1000000;

  return elapsed_time;
}

Arena turn_arena;  // arena with a lifetime of just the current turn;

transposition_table_t
    transposition_table;  // transposition table, is reset each turn

void init_id() { turn_arena = (Arena){malloc(10 * 1000 * 1000), 0}; }

Move best_move_id(const Board* board, size_t time_limit) {
  active_bot = 0;
  dbg_mm();

  init_clock();
  size_t max_depth = 0;
  Move move_buffer[256];
  Slice(Move) buffer = (Slice(Move)){move_buffer, lengthof(move_buffer)};
  Slice(Move) moves = generate_legal_moves(buffer, board);
  Move best_move_so_far = moves.buffer[0];
  do {
    arena_clear(&turn_arena);
    transposition_table_init(&turn_arena, &transposition_table,
                             1009);  // resets transposition table
    double best_eval = -1000000;
    Move best_move = moves.buffer[0];
    // size_t best_move_index = 0;
    for (size_t i = 0; i < moves.length; i++) {
      Move curr_move = moves.buffer[i];
      Board board_after_move = copy_make_move(board, curr_move);
      double eval = -alpha_beta_id(&board_after_move, -1000000, -best_eval,
                                   max_depth, time_limit);
      if (get_time_passed() >= time_limit) {
        break;
      }
      if (eval > best_eval) {
        best_eval = eval;
        best_move = curr_move;
        // best_move_index = i;
      }
    }
    if (get_time_passed() >= time_limit) break;

    best_move_so_far = best_move;
    max_depth++;
    dbg_tds();

    // moves.buffer[best_move_index] = moves.buffer[0];
    // moves.buffer[0] = best_move;
  } while (1);
  return best_move_so_far;
}

// pvs
Move best_move_id_new(const Board* board, size_t time_limit) {
  active_bot = 1;
  dbg_mm();
  init_clock();
  size_t max_depth = 0;
  Move move_buffer[256];
  Slice(Move) buffer = (Slice(Move)){move_buffer, lengthof(move_buffer)};
  Slice(Move) moves = generate_legal_moves(buffer, board);
  Move best_move_so_far = moves.buffer[0];
  do {
    arena_clear(&turn_arena);
    transposition_table_init(&turn_arena, &transposition_table,
                             1009);  // resets transposition table
    double best_eval = -1000000;
    Move best_move = moves.buffer[0];
    size_t best_move_index = 0;
    for (size_t i = 0; i < moves.length; i++) {
      Move curr_move = moves.buffer[i];
      Board board_after_move = copy_make_move(board, curr_move);
      double eval = -alpha_beta_id(&board_after_move, -1000000, -best_eval,
                                   max_depth, time_limit);
      if (get_time_passed() >= time_limit) {
        break;
      }
      if (eval > best_eval) {
        best_eval = eval;
        best_move = curr_move;
        best_move_index = i;
      }
    }
    if (get_time_passed() >= time_limit) break;

    best_move_so_far = best_move;
    max_depth++;
    dbg_tds();

    moves.buffer[best_move_index] = moves.buffer[0];
    moves.buffer[0] = best_move;
  } while (1);

  return best_move_so_far;
}

double alpha_beta_id(const Board* board, double alpha, double beta,
                     size_t max_depth, size_t time_limit) {
  dbg_sb();
  size_t time_passed = get_time_passed();
  if (time_passed >= time_limit) {
    return 0.0;
  }
  transposition_table_item_t* node =
      transposition_table_lookup(&transposition_table, board);
  if (node) {
    return node->evaluation;
  }
  if (max_depth == 0) {
    return quiescence_search(&turn_arena, &transposition_table, board, alpha,
                             beta);
  }

  Move move_buffer[256];
  Slice(Move) buffer = (Slice(Move)){move_buffer, lengthof(move_buffer)};
  Slice(Move) moves = generate_legal_moves(buffer, board);
  if (moves.length == 0) {
    if (is_check(board)) {
      transposition_table_insert(&turn_arena, &transposition_table, board,
                                 -200.0);
      return (-200.0);
    } else {
      transposition_table_insert(&turn_arena, &transposition_table, board, 0.0);
      return (0.0);
    }
  }
  // moves = sort_moves_by_heuristic(board, moves, quick_eval_move);
  double best_eval = -10000;
  for (size_t i = 0; i < moves.length; i++) {
    Move curr_move = moves.buffer[i];
    Board cloned_board = *board;
    make_move(&cloned_board, curr_move);
    double eval =
        -alpha_beta_id(&cloned_board, -beta, -alpha, max_depth - 1, time_limit);
    if (eval >= beta) {
      dbg_bfh();
      transposition_table_insert(&turn_arena, &transposition_table, board,
                                 beta);
      return (beta);
    }
    if (eval > best_eval) {
      best_eval = eval;
      if (eval > alpha) {
        alpha = eval;
      }
    }
  }
  transposition_table_insert(&turn_arena, &transposition_table, board,
                             alpha);
  return (alpha);
}