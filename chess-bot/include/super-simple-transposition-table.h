#include "bot-types.h"

typedef struct _node {
  zobrist_t zobrist;
  uint16_t hash_move;       // compacted best move.
  score_centipawn_t score;  // score of the node, may not be exact check node
                            // type for details.
  uint8_t type;             // node type
  uint8_t depth;            // distance in ply from the root node to the node
  uint8_t age;              //
  uint16_t padding;         // padding to make sizeof(node_t) == 8
} node_t;

typedef struct _table {
  node_t* slots;
  size_t table_size;
  size_t replacement_strategy;
} table_t;

#define TTNODE_EMPTY 0
#define TTNODE_EXACT 1
#define TTNODE_LOWER 2
#define TTNODE_UPPER 3

#define TT_ALWAYS_REPLACE 0
#define TT_DEPTH_PREFERED 1

static inline uint16_t compress_move(move_t move) {
  return (sq0x88_to_sq8x8(get_to(move)) << 6) |
         (sq0x88_to_sq8x8(get_from(move)) << 0) | (get_flags(move) << 12);
}

static inline move_t uncompress_move(uint16_t packed_move) {
  return move(sq8x8_to_sq0x88((packed_move >> 0) & 0x3F),
              sq8x8_to_sq0x88((packed_move >> 6) & 0x3F),
              (packed_move >> 12) & 0xF);
}

static inline move_t node_get_hash_move(node_t* node) {
  return uncompress_move(node->hash_move);
}

static inline score_centipawn_t node_get_score(node_t* node) {
  return node->score;
}

static inline int node_get_type(node_t* node) { return (int)node->type; }

static inline int node_get_age(node_t* node) { return node->age; }

static inline int node_get_depth(node_t* node) { return node->depth; }

static inline int node_get_depth_searched(node_t* node) {
  return (int)node->age - node->depth;
}

void make_table(table_t* table, size_t size, int replacement_strategy);

void release_table(table_t* table);

// gets the node from the node buffer, if index is NULL_INDEX it returns a
// null node.
node_t* get_node(const table_t* table, zobrist_t zobrist);

// sets the node in the node buffer, if the the index is NULL_INDEX it does
// nothing.
void set_node(table_t* table, zobrist_t zobrist, move_t best_move,
              score_centipawn_t score, int type, int depth, int age);