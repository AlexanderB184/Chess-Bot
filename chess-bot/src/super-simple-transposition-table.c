#include "../include/super-simple-transposition-table.h"

#include <stdlib.h>

void make_table(table_t* table, size_t size, int replacement_strategy) {
  table->slots = calloc(sizeof(node_t), size);
  table->table_size = size;
  table->replacement_strategy = replacement_strategy;
}

void release_table(table_t* table) { free(table->slots); }

node_t* get_node(const table_t* table, zobrist_t zobrist) {
  node_t* slot = table->slots + zobrist % table->table_size;
  if (slot->type == TTNODE_EMPTY || slot->zobrist != zobrist) {
    return NULL;
  }
  return slot;
}

void set_node(table_t* table, zobrist_t zobrist, move_t best_move,
              score_centipawn_t score, int type, int depth, int age) {
  node_t* slot = table->slots + zobrist % table->table_size;
  int replace = 0;
  switch (table->replacement_strategy) {
    case TT_ALWAYS_REPLACE:
      replace = 1;
      break;
    case TT_DEPTH_PREFERED:
      replace =
          slot->type == TTNODE_EMPTY || slot->age - slot->depth < age - depth;
      break;
    default:
      break;
  }
  if (replace) {
    *slot = (node_t){
        .zobrist = zobrist,
        .hash_move = compress_move(best_move),
        .type = type,
        .depth = depth,
        .age = age,
        .score = score,
    };
  }
}