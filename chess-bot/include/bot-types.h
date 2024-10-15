#ifndef BOT_TYPES_H
#define BOT_TYPES_H

#include <stdint.h>
#include <time.h>
#include <stdatomic.h>

#include "../chess-lib/include/chess-lib.h"

#define CHECKMATE_SCORE_CENTIPAWNS (score_cp_t)(-16384)
#define STALEMATE_SCORE_CENTIPAWNS (score_cp_t)(0)
#define DRAW_SCORE_CENTIPAWNS (score_cp_t)(0)
#define MAX_SCORE (score_cp_t)(INT16_MAX)
#define MIN_SCORE (score_cp_t)(-INT16_MAX)

typedef int16_t score_cp_t;

typedef struct {
  int64_t node_limit_nds;   // max nodes to search (-1 for no limit)
  int64_t time_limit_ms;    // max time to search in ms (-1 for no limit)
  int64_t depth_limit_ply;  // max depth to search in ply
} bot_term_cond_t;



typedef struct {
  int nthreads;
} bot_settings_t;

typedef struct {
  score_cp_t score;
} root_move_t;

typedef struct {
  chess_state_t position;
  size_t move_count;
  move_t* moves;
  root_move_t* root_moves;

  volatile int* abort;
  bot_t* bot;
} thread_data_t;

typedef struct {
  int nthreads;
  thread_data_t** threads;

  volatile int abort;
  bot_term_cond_t stop_cond;

  void* tt; // shared transposition table

  struct timespec start_time;
  atomic_uintmax_t nodes_searched;
} bot_t;

#endif