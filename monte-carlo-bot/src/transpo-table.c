#include "../include/transpo-table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t table_max_items = 10 * 1000 * 1000;
const static node_t null_node = {.type = EMPTY_NODE};

uint16_t compress_move(move_t move) {
  return (sq0x88_to_sq8x8(get_to(move)) << 6) |
         (sq0x88_to_sq8x8(get_from(move)) << 0) | 
         (get_flags(move) << 12);
}

move_t uncompress_move(uint16_t packed_move) {
  return move(sq8x8_to_sq0x88((packed_move >> 0) & 0x3F),
              sq8x8_to_sq0x88((packed_move >> 6) & 0x3F),
              (packed_move >> 12) & 0xF);
}

size_t next_prime(size_t n) {
  while (1) {
    int is_prime = 1;
    for (size_t i = 2; i * i <= n; i++) {
      if (n % i == 0) {
        is_prime = 0;
        break;
      }
    }
    if (is_prime) return n;
    n++;
  }
}

node_t make_node(move_t hash_move, score_centipawn_t score, int node_type,
                 int age, int depth) {
  return (node_t){
      .hash_move = compress_move(hash_move),
      .score = score,
      .type = node_type,
      .age = age,
      .depth = depth,
  };
}

node_t get_node(const table_t* table, node_index_t index) {
  if (index == NULL_INDEX) {
    return null_node;
  }
  return table->item_buffer[index];
}

void set_node(table_t* table, node_index_t index, node_t node) {
  if (index == NULL_INDEX) {
    return;
  }
  table->item_buffer[index] = node;
}

int is_null_node(node_t node) {
  return node.type == EMPTY_NODE;
}


move_t node_get_hash_move(node_t node) {
  return uncompress_move(node.hash_move);
}

score_centipawn_t node_get_score(node_t node) { return node.score; }

node_type_t node_get_type(node_t node) { return (node_type_t)node.type; }

int node_get_age(node_t node) { return node.age; }

int node_get_depth(node_t node) { return node.depth; }

int node_get_depth_searched(node_t node) { return (int)node.age - node.depth; }

// #define DEBUG
#ifdef DEBUG
#define DEBUG_MEMORY_USAGE(table, outstream)                                \
  {                                                                         \
    size_t debug_memory_usage = sizeof(table_t) +                           \
                                (table)->bucket_count * sizeof(bucket_t) +  \
                                (table)->item_capacity * sizeof(node_t);    \
    for (size_t debug_index = 0; debug_index < (table)->bucket_count;       \
         debug_index++) {                                                   \
      debug_memory_usage +=                                                 \
          (table)->buckets[debug_index].capacity * sizeof(slot_t);          \
    }                                                                       \
    fprintf((outstream),                                                    \
            "DEBUG: table %p is using %zu bytes for %zu items. (%0.2f per " \
            "node)\n",                                                      \
            (table), debug_memory_usage, (table)->item_count,               \
            ((double)debug_memory_usage / (table)->item_count));            \
  }
#else
#define DEBUG_MEMORY_USAGE(table, outstream)
#endif

void buffer_return(bucket_t* bucket) {
  if (bucket->items) free(bucket->items);
}

void buffer_init(bucket_t* bucket) {
  static const uint32_t init_size = 1;
  bucket->capacity = init_size;
  bucket->items = (slot_t*)malloc(init_size * sizeof(slot_t));
}

void buffer_expand(bucket_t* bucket) {
  size_t new_size = bucket->capacity * 2;
  bucket->capacity = new_size;
  bucket->items = (slot_t*)realloc(bucket->items, new_size * sizeof(slot_t));
}

bucket_t* ensure_space_for_items(bucket_t* bucket) {
  if (bucket->capacity == 0) {
    buffer_init(bucket);
  } else if (bucket->count >= bucket->capacity) {
    buffer_expand(bucket);
  }
  return bucket;
}

void make_table(table_t* table, size_t capacity, size_t bucket_count) {
  table->bucket_count = bucket_count;
  table->buckets = calloc(table->bucket_count, sizeof(bucket_t));
  table->item_capacity = capacity;
  table->item_buffer = malloc(table->item_capacity * sizeof(node_t));
  table->item_count = 0;
}

void release_table(table_t* table) {
  for (size_t i = 0; i < table->bucket_count; i++) {
    buffer_return(&table->buckets[i]);
  }
  free(table->buckets);
  free(table->item_buffer);
}

void clear_table(table_t* table) {
  table->item_count = 0;
  for (size_t i = 0; i < table->bucket_count; i++) {
    table->buckets[i].count = 0;
  }
}

int rehash(table_t* table) {
  DEBUG_MEMORY_USAGE(table, stdout);
  size_t old_bucket_count = table->bucket_count;
  size_t new_bucket_count = next_prime(old_bucket_count * 2);
  bucket_t* new_buckets = (bucket_t*)calloc(new_bucket_count, sizeof(bucket_t));

  for (size_t i = 0; i < old_bucket_count; i++) {
    bucket_t* old_bucket = &table->buckets[i];

    for (size_t j = 0; j < old_bucket->count; j++) {
      slot_t item = old_bucket->items[j];

      size_t new_bucket_index = item.key % table->bucket_count;

      bucket_t* new_bucket = &new_buckets[new_bucket_index];

      ensure_space_for_items(new_bucket);

      new_bucket->items[new_bucket->count++] = item;
    }

    buffer_return(old_bucket);
  }
  DEBUG_MEMORY_USAGE(table, stdout);

  bucket_t* old_buckets = table->buckets;

  table->buckets = new_buckets;
  table->bucket_count = new_bucket_count;

  free(old_buckets);

  return 0;
}

int ensure_capacity(table_t* table) {
  static const size_t load_factor_threshold = 3;

  if (table->item_count >= table_max_items) {
    return 1;
  }

  if (table->item_count >= table->item_capacity) {
    size_t new_capacity = table->item_capacity * 2;
    table->item_buffer =
        (node_t*)realloc(table->item_buffer, new_capacity * sizeof(node_t));
    table->item_capacity = new_capacity;
  }

  if (table->item_count > table->bucket_count * load_factor_threshold) {
    return rehash(table);
  }

  return 0;
}

node_index_t insert_table(table_t* table, zobrist_t key, node_t node) {
  if (ensure_capacity(table)) {
    return NULL_INDEX;
  }

  size_t bucket_index = key % table->bucket_count;

  bucket_t* bucket = &table->buckets[bucket_index];

  ensure_space_for_items(bucket);

  node_index_t node_index = table->item_count++;

  table->item_buffer[node_index] = node;

  bucket->items[bucket->count++] = (slot_t){
      .key = key,
      .index = node_index,
  };

  return node_index;
}

node_index_t search_table(const table_t* table, zobrist_t key) {
  size_t bucket_index = key % table->bucket_count;
  bucket_t bucket = table->buckets[bucket_index];
  for (size_t i = 0; i < bucket.count; i++) {
    if (bucket.items[i].key == key) {
      return bucket.items[i].index;
    }
  }
  return NULL_INDEX;
}

node_index_t search_or_insert_table(table_t* table, zobrist_t key) {
  int space_for_more_items = !ensure_capacity(table);
  size_t bucket_index = key % table->bucket_count;
  bucket_t* bucket = &table->buckets[bucket_index];
  for (size_t i = 0; i < bucket->count; i++) {
    if (bucket->items[i].key == key) {
      return bucket->items[i].index;
    }
  }
  if (!space_for_more_items) {
    return NULL_INDEX;
  }

  ensure_space_for_items(bucket);

  node_index_t node_index = table->item_count++;

  table->item_buffer[node_index] = null_node;

  bucket->items[bucket->count++] = (slot_t){
      .key = key,
      .index = node_index,
  };

  return node_index;
}

int update_policy(node_t old_node, node_t new_node) {
  int old_depth_searched = node_get_depth_searched(old_node);
  int new_depth_searched = node_get_depth_searched(new_node);
  return (node_get_type(old_node) == EMPTY_NODE) ||
         (new_depth_searched >= old_depth_searched);
}

int insertion_policy(node_t new_node) { return 1; }

int update_table(table_t* table, node_index_t index, zobrist_t key,
                 node_t new_node) {
  if (index == NULL_INDEX) {
    return 0;
  }
  if (update_policy(table->item_buffer[index], new_node)) {
    table->item_buffer[index] = new_node;
  }
  return 1;
}

int update_or_insert_table(table_t* table, node_index_t index, zobrist_t key,
                           node_t new_node) {
  if (index == NULL_INDEX) {
    index = search_table(table, key);
  }
  if (index == NULL_INDEX) {
    if (insertion_policy(new_node)) {
      return insert_table(table, key, new_node) != NULL_INDEX;
    } else {
      return 0;
    }
  }
  if (update_policy(table->item_buffer[index], new_node)) {
    table->item_buffer[index] = new_node;
  }
  return 1;
}