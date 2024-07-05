#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include "../../chess-lib/include/chess-lib.h"
#include "bot-types.h"

typedef enum {
  EMPTY_NODE = 0,
  PV_NODE,
  CUT_NODE,
  ALL_NODE,
} node_type_t;

typedef struct {
  zobrist_t key;
  score_centipawn_t score;
  uint16_t refutation_move;
  uint32_t age_depth_type_bitfield;
} node_t;


typedef struct {
  node_t* buckets;
  size_t bucket_count;
  size_t item_count;
} table_t;

extern table_t transposition_table;

size_t load_factor(const table_t*);

node_t node(zobrist_t key, score_centipawn_t score, uint32_t type,
            move_t refutation, uint32_t age, uint32_t depth);
zobrist_t get_key(node_t);
score_centipawn_t get_score(node_t);
uint32_t get_age(node_t);
uint32_t get_depth(node_t);
uint32_t get_type(node_t);
move_t get_refutation(node_t);
node_t* get_next(node_t);
node_t* find_in_buckets(node_t* buckets, size_t bucket_count, zobrist_t key);
void new_table(table_t* table, size_t starting_capacity);
int expand_table(table_t* table, size_t starting_capacity);
void release_table(table_t* table);
node_t* insert_table(table_t* table, zobrist_t key, score_centipawn_t value,
                     uint32_t type, move_t refutation, uint32_t age,
                     uint32_t depth);
node_t* update_node(node_t* node, score_centipawn_t value, uint32_t type,
                    move_t refutation, uint32_t age, uint32_t depth);
node_t* lookup_table(table_t* table, zobrist_t key);
void clear_table(table_t* table);

node_t* update_table(table_t* table, zobrist_t key, score_centipawn_t score,
                     uint32_t type, move_t refutation, uint32_t age,
                     uint32_t depth);

#endif