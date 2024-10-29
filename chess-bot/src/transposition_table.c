#include "../include/transposition_table.h"

// layout
// 0..2 type
// 2..16 unused
// 16..24 age
// 24..32 depth
// 32..48 score
// 48..64 best move

move_t entry_best_move(entry_t entry) {
  uint16_t compressed_move = ((entry >> 48) & 0xFFFF);
  return move(compressed_move & 0x3F, ((compressed_move >> 6) & 0x3F),
              ((compressed_move >> 12) & 0xF));
}

score_cp_t entry_score(entry_t entry) {
  return (score_cp_t)((entry >> 32) & 0xFFFF);
}

enum tt_entry_type entry_type(entry_t entry) {
  return (enum tt_entry_type)(entry & 0b11);
}

int entry_depth(entry_t entry) { return (int)((entry >> 16) & 0xFF); }

int entry_age(entry_t entry) { return (int)((entry >> 24) & 0xFF); }

entry_t make_entry(enum tt_entry_type type, move_t best_move, score_cp_t score,
                   int depth, int age) {
  return (entry_t)((entry_t)(type & 0b11) | ((entry_t)(age & 0xFF) << 16) |
                   ((entry_t)(depth & 0xFF) << 24) | ((entry_t)(score & 0xFFFF) << 32) |
                   (((entry_t)(best_move.from & 0x3F) | ((entry_t)(best_move.to & 0x3F) << 6) |
                     ((entry_t)(best_move.bitpacked_data & 0xF) << 12))
                    << 48));
}

void tt_init(table_t* table, uint64_t capacity) {
  table->items = calloc(capacity, sizeof(key_entry_pair_t));
  table->capacity = capacity;
}

void tt_free(table_t* table) { free((void*)table->items); }

entry_t tt_get(table_t* table, zobrist_t key) {
    uint64_t index = key % table->capacity;
    zobrist_t stored_key = table->items[index].key;
    uint64_t stored_entry = table->items[index].entry;
    if ((stored_key ^ stored_entry) == key) {
        return stored_entry;
    }
    return (entry_t)0;
}

void tt_store(table_t* table, zobrist_t key, enum tt_entry_type type, move_t best_move, score_cp_t score, int depth, int age) {
    uint64_t index = key % table->capacity;
    entry_t entry = make_entry(type, best_move, score, depth, age);
    table->items[index].key = key ^ entry;
    table->items[index].entry = entry;
}

void tt_store_depth_prefered(table_t* table, zobrist_t key, enum tt_entry_type type, move_t best_move, score_cp_t score, int depth, int age) {
    uint64_t index = key % table->capacity;
    uint64_t stored_entry = table->items[index].entry;
    if (entry_depth(stored_entry) <= depth) {
        entry_t entry = make_entry(type, best_move, score, depth, age);
        table->items[index].key = key ^ entry;
        table->items[index].entry = entry;
    }
}

void tt_store_pv(table_t* table, zobrist_t key, enum tt_entry_type type, move_t best_move, score_cp_t score, int depth, int age) {
    if (type == TT_EXACT) {
        uint64_t index = key % table->capacity;
        entry_t entry = make_entry(type, best_move, score, depth, age);
        table->items[index].key = key ^ entry;
        table->items[index].entry = entry;
    }
}