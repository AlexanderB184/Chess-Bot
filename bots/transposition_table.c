#include "transposition_table.h"

#include <math.h>

#include "debug.h"
#include "primes.h"
#include "zobrist.h"

dbg_header();

size_t next_prime(size_t n) {
  size_t start = 0, end = lengthof(primes) - 1;
  while (start != end) {
    size_t mid = (start + end) / 2;
    if (primes[mid] > n) {
      start = mid;
    } else if (primes[mid] < n) {
      end = mid;
    } else {
      return mid;
    }
  }
  return primes[end];
}

zobrist_t zobrist(const Board* board) {
  zobrist_t zobrist_hash = 0;
  if (board->turn == BLACK) {
    zobrist_hash ^= black_to_move_zobrist;
  }
  for (size_t square = 0; square < 8; square++) {
    Piece piece = board_get_piece(board, square);
    if (piece.type == EMPTY) continue;
    int zobrist_table_piece_index = piece.type - 1;
    if (piece.colour == BLACK) {
      zobrist_table_piece_index += 6;
    }
    zobrist_hash ^= zobrist_table[zobrist_table_piece_index][square];
  }
  return zobrist_hash;
}

#ifdef TRANSPOSITION_TABLE_STRICT_BOARD_COMPARISON
int board_cmp(const Board* left_board, const Board* right_board) {
  if (left_board->turn != right_board->turn) {
    return 0;
  }
  if (left_board->enpassent != right_board->enpassent) {
    return 0;
  }
  if (left_board->rights != right_board->rights) {
    return 0;
  }
  if (memcmp(left_board->pieces, right_board->pieces,
             sizeof(left_board->pieces))) {
    return 0;
  }
  return 1;
}
#else
#define board_cmp(left_board, right_board) 1 /*Currently disabled*/
#endif

void transposition_table_init(Arena* arena, transposition_table_t* table,
                              size_t starting_buckets) {
  table->buckets = arena_push_array_zero(arena, transposition_table_item_t*,
                                         starting_buckets);
  table->bucket_count = starting_buckets;
  table->item_count = 0;
}

transposition_table_item_t* transposition_table_lookup(
    const transposition_table_t* table, const Board* board) {
  zobrist_t zobrist_hash = zobrist(board);
  size_t index = zobrist_hash % table->bucket_count;
  transposition_table_item_t* node = table->buckets[index];
  for (; node; node = node->next) {
    if (zobrist_hash == node->zobrist_hash && board_cmp(board, node->board)) {
      dbg_tth();
      return node;
    }
  }
  dbg_ttm();

  return NULL;
}

void transposition_table_insert(Arena* arena, transposition_table_t* table,
                                const Board* board, double evaluation) {
  zobrist_t zobrist_hash = zobrist(board);

  // expand buckets
  if (++table->item_count > table->bucket_count * 4 / 5) {
    size_t new_bucket_count = table->bucket_count * 3 / 2;
    transposition_table_item_t** new_buckets = arena_push_array_zero(
        arena, transposition_table_item_t*, new_bucket_count);
    for (size_t index = 0; index < table->bucket_count; index++) {
      for (transposition_table_item_t *next, *node = table->buckets[index];
           node; node = next) {
        next = node->next;
        size_t new_index = node->zobrist_hash % new_bucket_count;
        node->next = new_buckets[new_index];
        new_buckets[new_index] = node;
      }
    }
    table->buckets = new_buckets;
    table->bucket_count = new_bucket_count;
  }

  size_t index = zobrist_hash % table->bucket_count;
  transposition_table_item_t* node =
      arena_push_struct_zero(arena, transposition_table_item_t);

  node->next = table->buckets[index];
  node->zobrist_hash = zobrist_hash;
#ifdef TRANSPOSITION_TABLE_STRICT_BOARD_COMPARISON
  node->board = *board;
#endif
  node->evaluation = evaluation;

  table->buckets[index] = node;
}