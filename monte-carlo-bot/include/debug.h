#include <stddef.h>
#include <string.h>

//#define DEBUG

typedef struct {
  size_t max_depth_reached;
  size_t moves_evaluated_in_final_iteration;
  size_t possible_moves;

  size_t unique_nodes_searched;
  size_t total_nodes_searched;
  size_t check_extension_nodes_searched;
  size_t quiescence_nodes_searched;

  size_t transposition_table_cutoffs;
  size_t beta_cutoffs;
  size_t delta_cutoffs;

  size_t time_elapsed_ms;

} debug_info_t;

extern debug_info_t debug_info;

#ifdef DEBUG
#define DEBUG_CLEAR() memset(&debug_info, 0, sizeof(debug_info));
#define DEBUG_SET_MAX_DEPTH_REACHED(MAX_DEPTH) debug_info.max_depth_reached = (MAX_DEPTH);
#define DEBUG_SET_POSSIBLE_MOVES(MOVE_COUNT) debug_info.possible_moves = (MOVE_COUNT);
#define DEBUG_SET_MOVES_EVALUATED_FINAL_ITER(MOVES_EVALUATED) debug_info.moves_evaluated_in_final_iteration = (MOVES_EVALUATED);
#define DEBUG_SET_UNIQUE_NODES_SEARCHED(UNIQUE_NODES_SEARCHED) debug_info.unique_nodes_searched = (UNIQUE_NODES_SEARCHED);
#define DEBUG_SET_TOTAL_NODES_SEARCHED(TOTAL_NODES_SEARCHED) debug_info.total_nodes_searched = (TOTAL_NODES_SEARCHED);
#define DEBUG_INC_TOTAL_NODES_SEARCHED() debug_info.total_nodes_searched++;
#define DEBUG_SET_TIME_ELAPSED(TIME_ELAPSED) debug_info.time_elapsed_ms = (TIME_ELAPSED);
#define DEBUG_SET_TRANSPO_CUTOFFS(CUTOFFS) debug_info.transposition_table_cutoffs = (CUTOFFS);
#define DEBUG_SET_BETA_CUTOFFS(CUTOFFS) debug_info.beta_cutoffs = (CUTOFFS);
#define DEBUG_INC_TRANSPO_CUTOFFS() debug_info.transposition_table_cutoffs++;
#define DEBUG_INC_BETA_CUTOFFS() debug_info.beta_cutoffs++;
#define DEBUG_INC_DELTA_CUTOFFS() debug_info.delta_cutoffs++;
#define DEBUG_INC_QUIESCENCE_NODES() debug_info.quiescence_nodes_searched++;
#define DEBUG_INC_CHECK_EXTENSION_NODES() debug_info.check_extension_nodes_searched++;
#else
#define DEBUG_CLEAR()
#define DEBUG_SET_POSSIBLE_MOVES(_)
#define DEBUG_SET_MAX_DEPTH_REACHED(_)
#define DEBUG_SET_MOVES_EVALUATED_FINAL_ITER(_)
#define DEBUG_SET_UNIQUE_NODES_SEARCHED(_)
#define DEBUG_SET_TOTAL_NODES_SEARCHED(_)
#define DEBUG_INC_TOTAL_NODES_SEARCHED()
#define DEBUG_SET_TIME_ELAPSED(_)
#define DEBUG_SET_TRANSPO_CUTOFFS(_)
#define DEBUG_SET_BETA_CUTOFFS(_)
#define DEBUG_INC_TRANSPO_CUTOFFS()
#define DEBUG_INC_BETA_CUTOFFS()
#define DEBUG_INC_DELTA_CUTOFFS()
#define DEBUG_INC_QUIESCENCE_NODES()
#define DEBUG_INC_CHECK_EXTENSION_NODES()


#endif