#include <time.h>

#include "../include/MCTS.h"
#include "../include/monte-carlo-bot.h"

struct timespec start_time;
size_t time_limit_ms = 100;

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 0
#endif

void start_timer() { clock_gettime(CLOCK_MONOTONIC, &start_time); }

size_t elapsed_time_ms() {
  struct timespec current_time;
  clock_gettime(CLOCK_MONOTONIC, &current_time);
  size_t elapsed_time = (current_time.tv_sec - start_time.tv_sec) * 1000;
  elapsed_time += (current_time.tv_nsec - start_time.tv_nsec) / 1000000;
  return elapsed_time;
}

int stop() { return elapsed_time_ms() >= time_limit_ms; }

move_t find_best_move(const chess_state_t* chess_state) {
  MCTS_tree_t search_tree;
  start_timer();
  MCTS_init_tree(&search_tree, chess_state);
  do {
    MCTS_run(&search_tree);
  } while (!stop());
  move_t best_move = MCTS_best_move(&search_tree);
  MCTS_release_tree(&search_tree);
  return best_move;
}