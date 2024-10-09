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

void traverse_tree(const MCTS_tree_t* tree, const MCTS_node_t* node, int depth);

#include <stdio.h>
move_t find_best_move(const chess_state_t* chess_state) {
  MCTS_tree_t search_tree;
  start_timer();
  MCTS_init_tree(&search_tree, chess_state);
  do {
    MCTS_run(&search_tree);
  } while (!stop());
  //printf("tree size: %u\n", search_tree.node_count);
  //traverse_tree(&search_tree, search_tree.node_buffer, 0);
  move_t best_move = MCTS_best_move(&search_tree);
  MCTS_release_tree(&search_tree);
  return best_move;
}


#include "../../chess-lib/include/chess-lib.h"

void traverse_tree(const MCTS_tree_t* tree, const MCTS_node_t* node,
                   int depth) {
  if (depth > 1) return;
  for (int i = 0; i < depth; i++) {
    printf("\t");
  }
  printf(
      "{score: %f, traversals: %u, avg score: %f, traversed children: %f move: "
      "%c%c%c%c, is_end_node: %d, children: [",
      node->score, node->traversals, node->score / node->traversals,
      node->traversed_children == 0
          ? 0
          : (float)node->traversed_children / node->children_count,
      sq0x88_to_file07(node->move.from) + 'a',
      sq0x88_to_rank07(node->move.from) + '1',
      sq0x88_to_file07(node->move.to) + 'a',
      sq0x88_to_rank07(node->move.to) + '1', node->end_node);

  if (depth == 0) {
    printf("\n");
  }

  for (int i = 0; i < node->children_count; i++) {
    traverse_tree(tree, &tree->node_buffer[node->first_child_index + i],
                  depth + 1);
  }
  //if (node->traversed_children > 0) {
  //  for (int i = 0; i < depth; i++) {
  //    printf("\t");
  //  }
  //}

  printf("]},\n");
}
