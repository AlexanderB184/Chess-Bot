#ifndef MCTS_H
#define MCTS_H

#include <stddef.h>
#include <stdint.h>
#include "../../chess-lib/include/chess-lib-types.h"

typedef struct MCTS_tree_t {
  struct MCTS_node_t* node_buffer;
  uint32_t buffer_capacity;
  uint32_t node_count;

  const chess_state_t* root_position;
  chess_state_t cursor_position;
} MCTS_tree_t;

typedef struct MCTS_node_t {
  uint8_t children_count;
  uint8_t traversed_children;

  uint8_t end_node; // could be merged into first child index
  uint8_t black_to_move; // not needed?

  move_t move; // could be reduced to 2 bytes

  uint32_t traversals;
  float score;

  uint32_t first_child_index; 
  uint32_t parent_index; // could be stored on a stack in the cursor
} MCTS_node_t;

typedef struct MCTS_cursor_t {
  struct MCTS_tree_t* tree;
  chess_state_t* board_position;
  uint32_t node_index;
} MCTS_cursor_t;

void MCTS_init_tree(struct MCTS_tree_t* tree, const chess_state_t* game);

void MCTS_release_tree(struct MCTS_tree_t* tree);

void MCTS_init_cursor(struct MCTS_tree_t* tree, struct MCTS_cursor_t* cursor);

void MCTS_release_cursor(struct MCTS_cursor_t* cursor);

// returns the parent of the selected leaf node
struct MCTS_cursor_t* MCTS_select(struct MCTS_cursor_t* cursor);

// adds the leaf node to the tree
struct MCTS_cursor_t* MCTS_expand(struct MCTS_cursor_t* cursor);

// simulates the position and returns the score
float MCTS_simulate(struct MCTS_cursor_t* cursor);

struct MCTS_cursor_t* MCTS_backpropogate(struct MCTS_cursor_t* cursor, float score);

int MCTS_run(struct MCTS_tree_t* tree);

move_t MCTS_best_move(const struct MCTS_tree_t* tree);

void MCTS_set_as_root(struct MCTS_tree_t* tree, struct MCTS_cursor_t* cursor);


#endif