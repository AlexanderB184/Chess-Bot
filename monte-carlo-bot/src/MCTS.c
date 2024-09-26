#include "../include/MCTS.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../chess-lib/include/chess-lib.h"
#include "../include/quiescence-search.h"
#include "../include/static-evaluation.h"

double uniform(double min, double max) {
  double scale = rand() / (double)RAND_MAX; /* [0, 1.0] */
  return min + scale * (max - min);         /* [min, max] */
}

struct MCTS_node_t* MCTS_node(struct MCTS_cursor_t* cursor) {
  return &cursor->tree->node_buffer[cursor->node_index];
}

struct MCTS_node_t* MCTS_children(struct MCTS_cursor_t* cursor) {
  MCTS_node_t* node = MCTS_node(cursor);
  return &cursor->tree->node_buffer[node->first_child_index];
}

struct MCTS_cursor_t* goto_child(struct MCTS_cursor_t* cursor, int index) {
  MCTS_node_t* node = MCTS_node(cursor);

  cursor->node_index = node->first_child_index + index;

  node = MCTS_node(cursor);

  make_move(cursor->board_position, node->move);

  return cursor;
}

struct MCTS_cursor_t* goto_parent(struct MCTS_cursor_t* cursor) {
  MCTS_node_t* node_buffer = cursor->tree->node_buffer;
  MCTS_node_t* node = &node_buffer[cursor->node_index];

  cursor->node_index = node->parent_index;

  unmake_move(cursor->board_position);

  return cursor;
}

struct MCTS_cursor_t* goto_parent_quick(struct MCTS_cursor_t* cursor) {
  MCTS_node_t* node_buffer = cursor->tree->node_buffer;
  MCTS_node_t* node = &node_buffer[cursor->node_index];

  cursor->node_index = node->parent_index;

  return cursor;
}

double MCTS_node_weight(struct MCTS_node_t* child,
                        double weight_parent_component) {
  return child->score / child->traversals +
         weight_parent_component / sqrt(child->traversals);
}

void MCTS_init_tree(struct MCTS_tree_t* tree,
                    const chess_state_t* chess_state) {
  tree->root_position = chess_state;
  memset(&tree->cursor_position, 0, sizeof(tree->cursor_position));
  tree->node_count = 1;

  tree->buffer_capacity = 128;
  tree->node_buffer =
      (MCTS_node_t*)malloc(tree->buffer_capacity * sizeof(MCTS_node_t));

  tree->node_buffer[0] =
      (MCTS_node_t){.black_to_move = chess_state->black_to_move};
}

void MCTS_release_tree(struct MCTS_tree_t* tree) { free(tree->node_buffer); }

void MCTS_init_cursor(struct MCTS_tree_t* tree, struct MCTS_cursor_t* cursor) {
  copy_position(&tree->cursor_position, tree->root_position);
  cursor->board_position = &tree->cursor_position;
  cursor->tree = tree;
  cursor->node_index = 0;
}

void MCTS_release_cursor(struct MCTS_cursor_t* cursor) {
  clear_position(&cursor->tree->cursor_position);
}

struct MCTS_cursor_t* MCTS_select(struct MCTS_cursor_t* cursor) {
  struct MCTS_node_t* node = MCTS_node(cursor);

  while (node->children_count > 0 &&
         node->children_count == node->traversed_children) {
    struct MCTS_node_t* children = MCTS_children(cursor);
    double weight_parent_component = sqrt(2 * log(node->traversals));

    double total_weight;

    total_weight = MCTS_node_weight(children, weight_parent_component);

    uint32_t selected_node_index = 0;

    for (uint32_t i = 1; i < node->children_count; i++) {
      MCTS_node_t* child = &children[i];
      double weight = MCTS_node_weight(child, weight_parent_component);

      double r = uniform(0, total_weight + weight);
      if (r > total_weight) {
        selected_node_index = i;
        total_weight += weight;
      }
    }

    goto_child(cursor, selected_node_index);

    node = MCTS_node(cursor);
  }
  return cursor;
}

struct MCTS_node_t* MCTS_make_node(struct MCTS_cursor_t* cursor) {
  MCTS_node_t* node = MCTS_node(cursor);

  int gameover = is_gameover(cursor->board_position);

  if (gameover) {
    if (gameover == CHECKMATE) {
      node->score = 1;
    } else {
      node->score = 0.5;
    }
    node->end_node = 1;
    return node;
  }

  move_t moves[256];
  int move_count = generate_legal_moves(cursor->board_position, moves);
  MCTS_tree_t* tree = cursor->tree;

  if (tree->node_count + move_count >= tree->buffer_capacity) {
    tree->buffer_capacity = (tree->node_count + move_count) * 2;
    tree->node_buffer =
        realloc(tree->node_buffer, tree->buffer_capacity * sizeof(MCTS_node_t));
    node = MCTS_node(cursor);
  }

  node->first_child_index = tree->node_count;
  node->children_count = move_count;

  tree->node_count += move_count;

  MCTS_node_t* children = MCTS_children(cursor);

  memset(children, 0, move_count * sizeof(MCTS_node_t));

  for (int i = 0; i < move_count; i++) {
    children[i].parent_index = cursor->node_index;
    children[i].black_to_move = !cursor->board_position->black_to_move;
    children[i].move = moves[i];
  }

  return node;
}

struct MCTS_cursor_t* MCTS_expand(struct MCTS_cursor_t* cursor) {
  MCTS_node_t* node = MCTS_node(cursor);

  if (node->end_node) {
    return cursor;
  }

  if (node->children_count == 0) {
    node = MCTS_make_node(cursor);

    if (node->end_node) {
      return cursor;
    }
  }

  int i = node->traversed_children +
          rand() % (node->children_count - node->traversed_children);
  MCTS_node_t* children = MCTS_children(cursor);
  move_t move = children[i].move;
  children[i].move = children[node->traversed_children].move;
  children[node->traversed_children].move = move;

  goto_child(cursor, node->traversed_children++);

  return cursor;
}

float MCTS_simulate(struct MCTS_cursor_t* cursor) {
  struct MCTS_node_t* node = MCTS_node(cursor);

  if (node->end_node) {
    return node->score;
  }
  chess_state_t* game = cursor->board_position;
  score_centipawn_t centipawn_score =
      -quiescence_search(game, MIN_SCORE, MAX_SCORE, 0, 0);
  float win_chance = 1 / (1 + pow(10, centipawn_score / 400));

  return win_chance;
}

struct MCTS_cursor_t* MCTS_backpropogate(struct MCTS_cursor_t* cursor,
                                         float score) {
  uint32_t prev_index;
  do {
    MCTS_node_t* node = MCTS_node(cursor);
    if (node->end_node) {
      score = 1.0f - score;
      prev_index = cursor->node_index;
      goto_parent_quick(cursor);
      continue;
    }

    node->traversals++;

    node->score += score;

    score = 1.0f - score;
    prev_index = cursor->node_index;
    goto_parent_quick(cursor);

  } while (cursor->node_index != prev_index);

  return cursor;
}

int MCTS_run(struct MCTS_tree_t* tree) {
  struct MCTS_cursor_t cursor;
  MCTS_init_cursor(tree, &cursor);
  MCTS_select(&cursor);
  MCTS_expand(&cursor);

  int32_t score = MCTS_simulate(&cursor);
  MCTS_backpropogate(&cursor, score);
  MCTS_release_cursor(&cursor);

  return 0;
}

move_t MCTS_best_move(const struct MCTS_tree_t* tree) {
  MCTS_node_t* root = &tree->node_buffer[0];
  MCTS_node_t* children = &tree->node_buffer[root->first_child_index];
  double best_score = -INFINITY;
  move_t best_move = null_move;
  for (int i = 0; i < root->traversed_children; i++) {
    double avg_score = children[i].score / children[i].traversals;
    if (avg_score > best_score) {
      best_score = avg_score;
      best_move = children[i].move;
    }
  }
  return best_move;
}

void MCTS_push_nodes_back(struct MCTS_tree_t* tree, uint32_t index,
                          uint32_t parent, int count) {
  if (count == 0) {
    return;
  }

  MCTS_node_t* node_buffer = tree->node_buffer;

  memmove(&node_buffer[tree->node_count], &node_buffer[index],
          count * sizeof(MCTS_node_t));

  index = tree->node_count;

  tree->node_count += count;

  for (int i = 0; i < count; i++) {
    MCTS_node_t* node = &node_buffer[index + i];
    node->parent_index = parent;
    MCTS_push_nodes_back(tree, node->first_child_index, index,
                         node->children_count);
  }
}

void MCTS_set_as_root(struct MCTS_tree_t* tree, struct MCTS_cursor_t* cursor) {
  tree->node_count = 0;

  MCTS_push_nodes_back(tree, cursor->node_index, 0, 1);
}