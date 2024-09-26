#include "bot-types.h"

#define NULL_INDEX -1


typedef enum {
  EMPTY_NODE = 0, // node is uninitialised.
  PV_NODE, // node stores the exact value, value can always be used.
  CUT_NODE, // node stores a value which is the result from a beta cutoff, the score is less than or equal to the true score for the node.
  ALL_NODE, // node stores a value which is the result from an alpha cutoff, the score is strictly greater than the true score for the node.
} node_type_t;

typedef int64_t node_index_t;

uint16_t compress_move(move_t move);
move_t uncompress_move(uint16_t move);

typedef struct {
  uint16_t hash_move; // compacted best move.
  score_centipawn_t score; // score of the node, may not be exact check node type for details.
  uint8_t type; // node type
  uint8_t age; // age of node, set to max depth, meaning that higher values are newer nodes
  uint8_t depth; // distance in ply from the root node to the node
  uint8_t padding; // padding to make sizeof(node_t) == 8
} node_t;

typedef struct {
  zobrist_t key; // zobrist hash of board state
  node_index_t index;
} slot_t;

typedef struct {
  slot_t* items;
  uint32_t count;
  uint32_t capacity;
} bucket_t;

// struct for transposition table.
typedef struct {
  bucket_t* buckets;
  size_t bucket_count;
  node_t* item_buffer;
  size_t item_capacity;
  size_t item_count;
} table_t;

// constructor for node_t type, packs data into struct and compress move struct.
node_t make_node(move_t hash_move, score_centipawn_t score, int node_type,
                 int age, int depth);

// gets the node from the node buffer, if index is NULL_INDEX it returns a null
// node.
node_t get_node(const table_t* table, node_index_t index);

// sets the node in the node buffer, if the the index is NULL_INDEX it does
// nothing.
void set_node(table_t* table, node_index_t index, node_t node);

int is_null_node(node_t node);

// gets the hash_move from the node_t struct.
move_t node_get_hash_move(node_t node);

// gets the score from the node_t struct.
score_centipawn_t node_get_score(node_t node);

// gets the node type from the node_t struct.
node_type_t node_get_type(node_t node);

// gets the age from the node_t struct. age is the max depth so larger values
// mean it is newer.
int node_get_age(node_t node);

// gets the depth in ply from the root node of the node_t struct.
int node_get_depth(node_t node);

// gets the depth the node was search at, i.e. the distance in ply from node to
// the "leaf" nodes where it was statically evaluated.
int node_get_depth_searched(node_t node);

// initialises a table with the specified capacity.
void make_table(table_t* table, size_t capacity, size_t bucket_count);

// releases the memory allocated to the table.
void release_table(table_t* table);

// clears all data from the table.
void clear_table(table_t* table);

// searches the table. if the key is not found it returns NULL_INDEX.
node_index_t search_table(const table_t* table, zobrist_t key);

// searches the table. if the key is not found it inserts a new empty node into
// the node buffer and returns the index of it. If the table is at the max item
// limit it returns NULL_INDEX.
node_index_t search_or_insert_table(table_t* table, zobrist_t key);

// updates a node in the table. If the node does not exist in the table it does
// nothing. Only updates the node if the update policy is met.
int update_table(table_t* table, node_index_t index, zobrist_t key,
                 node_t new_node);

// updates a node in the table. If the node does not exist in the table it will
// insert it.
int update_or_insert_table(table_t* table, node_index_t index, zobrist_t key,
                           node_t new_node);

extern table_t transpo_table;
