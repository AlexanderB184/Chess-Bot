#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include <stdint.h>

#include "../chess.h"

typedef uint64_t zobrist_t;

// hashes a chess board with zobrist hashing
zobrist_t zobrist(const Board* board);

typedef struct transposition_table_item_t transposition_table_item_t;

struct transposition_table_item_t {
  transposition_table_item_t* next;
  zobrist_t zobrist_hash;
#ifdef TRANSPOSITION_TABLE_STRICT_BOARD_COMPARISON
  Board board;
#endif
  double evaluation;
};

struct transposition_table_t {
  size_t bucket_count, item_count;
  transposition_table_item_t** buckets;
};

typedef struct transposition_table_t transposition_table_t;

transposition_table_item_t* transposition_table_lookup(
    const transposition_table_t* table, const Board* board);

void transposition_table_insert(Arena* arena, transposition_table_t* table,
                                const Board* board, double evaluation);

void transposition_table_init(Arena* arena, transposition_table_t* table,
                              size_t starting_buckets);

#endif