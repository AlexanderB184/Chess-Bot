
#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include <stdlib.h>

#include "../../chess-lib/include/chess-lib.h"

enum tt_entry_type { TT_EMPTY = 0, TT_EXACT, TT_UPPER, TT_LOWER };

// bitpacked data for transposition table entry
// layout
// 0..2 type
// 2..16 unused
// 16..24 age
// 24..32 depth
// 32..48 score
// 48..64 best move
typedef uint64_t entry_t;

// key value paired stored using xor validation
// key value is key ^ entry so to check if key matches do key == entry.key ^
// entry.entry this guarantees correctness as if two threads write the key and
// entry for a node simultaneously it will be invalidated
// https://www.chessprogramming.org/Shared_Hash_Table#Xor
typedef struct key_entry_pair_t {
  zobrist_t key;
  uint64_t entry;
} key_entry_pair_t;

// lockless shared transposition table
// https://www.chessprogramming.org/Shared_Hash_Table#Xor
typedef struct table_t {
  volatile struct key_entry_pair_t* items;
  uint64_t capacity;
} table_t;

#include "bot.h"

move_t entry_best_move(entry_t entry);
score_cp_t entry_score(entry_t entry);
enum tt_entry_type entry_type(entry_t entry);
int entry_depth(entry_t entry);
int entry_age(entry_t entry);

entry_t make_entry(enum tt_entry_type type, move_t best_move, score_cp_t score,
                   int depth, int age);

void tt_init(table_t* table, uint64_t capacity);
void tt_free(table_t* table);

entry_t tt_get(table_t* table, zobrist_t key);

// stores using always replace policy
void tt_store(table_t* table, zobrist_t key, enum tt_entry_type type,
              move_t best_move, score_cp_t score, int depth, int age);

// stores using depth prefered policy
void tt_store_depth_prefered(table_t* table, zobrist_t key,
                             enum tt_entry_type type, move_t best_move,
                             score_cp_t score, int depth, int age);

// only stores node if it is an exact node
void tt_store_pv(table_t* table, zobrist_t key, enum tt_entry_type type,
                 move_t best_move, score_cp_t score, int depth, int age);

#endif