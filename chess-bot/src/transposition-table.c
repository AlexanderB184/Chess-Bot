#include "../include/transposition-table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../chess-lib/include/chess-lib-constants.h"
#include "../../chess-lib/include/chess-lib.h"

#define TABLE_MAX_SIZE 32 * 1000 * 1000 * sizeof(node_t)

table_t transposition_table;

uint16_t compress_move(move_t move) {
  return (get_flags(move) << 12) | (sq0x88_to_sq8x8(get_to(move)) << 6) |
         (sq0x88_to_sq8x8(get_from(move)) << 0);
}

move_t uncompress_move(uint16_t move) {
  return unordered_move(sq8x8_to_sq0x88((move >> 0) & 0x3F),
                        sq8x8_to_sq0x88((move >> 6) & 0x3F),
                        (move >> 12) & 0xF);
}

zobrist_t get_key(node_t node) { return node.key; }

score_centipawn_t get_score(node_t node) { return node.score; }

uint32_t get_age(node_t node) {
  return (node.age_depth_type_bitfield >> 8) & 0xFFF;
}

uint32_t get_depth(node_t node) {
  return (node.age_depth_type_bitfield >> 20) & 0xFFF;
}

uint32_t get_type(node_t node) { return (node.age_depth_type_bitfield) & 0b11; }

move_t get_refutation(node_t node) {
  return uncompress_move(node.refutation_move);
}

node_t node(zobrist_t key, score_centipawn_t score, uint32_t type,
            move_t refutation, uint32_t age, uint32_t depth) {
  uint32_t bitfield =
      (type & 0b11) | ((age & 0xFFF) << 8) | ((depth & 0xFFF) << 20);
  return (node_t){.key = key,
                  .score = score,
                  .age_depth_type_bitfield = bitfield,
                  .refutation_move = compress_move(refutation)};
}

size_t load_factor_percent(const table_t* table) {
  return (table->item_count * 100) / table->bucket_count;
}

int is_null_node(node_t node) { return get_type(node) == EMPTY_NODE; }

node_t* find_in_buckets(node_t* buckets, size_t bucket_count, zobrist_t key) {
  size_t index = key % bucket_count;
  node_t node = buckets[index];
  size_t k = 1;
  while (get_type(node) != EMPTY_NODE && get_key(node) != key) {
    index = (index + k * k) % bucket_count;
    k++;
    node = buckets[index];
  }
  return &buckets[index];
}

void new_table(table_t* table, size_t starting_capacity) {
  table->bucket_count = starting_capacity;
  table->buckets = (node_t*)calloc(table->bucket_count, sizeof(node_t));
  table->item_count = 0;
}

int expand_table(table_t* table, size_t extra_capacity) {
  static int expansions = 0;
  expansions++;
  if ((table->bucket_count + extra_capacity) * sizeof(node_t) >=
      TABLE_MAX_SIZE) {
    return 0;
  }
  node_t* old_buckets = table->buckets;
  size_t old_capacity = table->bucket_count;
  table->bucket_count += extra_capacity;
  table->buckets = (node_t*)calloc(table->bucket_count, sizeof(node_t));

  // printf("expanding table: %d, size: %zu, old size %zu, extra cap %zu\n",
  //        expansions, table->bucket_count, old_capacity, extra_capacity);

  for (size_t i = 0; i < old_capacity; i++) {
    if (get_type(old_buckets[i]) != EMPTY_NODE) {
      memcpy(find_in_buckets(table->buckets, table->bucket_count,
                             old_buckets[i].key),
             &old_buckets[i], sizeof(node_t));
    }
  }
  free(old_buckets);
  return 1;
}

void clear_table(table_t* table) {
  table->item_count = 0;
  memset(table->buckets, 0, table->bucket_count * sizeof(node_t));
}

void release_table(table_t* table) { free(table->buckets); }

node_t* insert_table(table_t* table, zobrist_t key, score_centipawn_t score,
                     uint32_t type, move_t refutation, uint32_t age,
                     uint32_t depth) {
  if (load_factor_percent(table) >= 75) {
    expand_table(table, 4 * table->bucket_count);
  }
  node_t* inserted_node =
      find_in_buckets(table->buckets, table->bucket_count, key);
  if (get_type(*inserted_node) == EMPTY_NODE) table->item_count++;
  *inserted_node = node(key, score, type, refutation, age, depth);
  return inserted_node;
}

node_t* update_node(node_t* node_to_update, score_centipawn_t score,
                    uint32_t type, move_t refutation, uint32_t age,
                    uint32_t depth) {
  *node_to_update =
      node(node_to_update->key, score, type, refutation, age, depth);
  return node_to_update;
}

int update_policy(node_t node, score_centipawn_t score, uint32_t type,
                  move_t refutation, uint32_t age, uint32_t depth) {
  return (get_type(node) == EMPTY_NODE) || (get_age(node) < age) ||
         (type == CUT_NODE && get_type(node) == CUT_NODE &&
          score > get_score(node)) ||
         (type == ALL_NODE && get_type(node) != PV_NODE) || (type == PV_NODE);
}

node_t* update_table(table_t* table, zobrist_t key, score_centipawn_t score,
                     uint32_t type, move_t refutation, uint32_t age,
                     uint32_t depth) {
  node_t* node_ptr = find_in_buckets(table->buckets, table->bucket_count, key);
  node_t node_value = *node_ptr;
  if (update_policy(node_value, score, type, refutation, age, depth)) {
    if (get_type(node_value) == EMPTY_NODE) {
      if (load_factor_percent(table) >= 75) {
        if (!expand_table(table, table->bucket_count)) {
          return NULL;
        } else {
          node_ptr = find_in_buckets(table->buckets, table->bucket_count, key);
        }
      }
      table->item_count++;
    }
    *node_ptr = node(key, score, type, refutation, age, depth);
  }
  return node_ptr;
}

node_t* lookup_table(table_t* table, zobrist_t key) {
  node_t* node = find_in_buckets(table->buckets, table->bucket_count, key);
  if (get_type(*node) != EMPTY_NODE) {
    return node;
  }
  return NULL;
}