
#ifndef BOT_H
#define BOT_H

#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "../include/chess.h"

#define CHECKMATE_SCORE_CENTIPAWNS (score_cp_t)(-16384)
#define STALEMATE_SCORE_CENTIPAWNS (score_cp_t)(0)
#define DRAW_SCORE_CENTIPAWNS (score_cp_t)(0)
#define MAX_SCORE (score_cp_t)(INT16_MAX)
#define MIN_SCORE (score_cp_t)(-INT16_MAX)

typedef int16_t score_cp_t;

#include "transposition_table.h"
#include "killer_moves.h"
#include "move_order.h"

struct match_state_t;
struct bot_settings_t;
struct bot_term_cond_t;
struct bot_t;
struct worker_t;

typedef struct bot_settings_t {
  int debug;   // debug flag
  int Hash;    // size of transposition table in MB
  int Ponder;  // whether pondering is enabled
  // other settings ...
} bot_settings_t;

typedef struct bot_term_cond_t {
  long time_limit_ms;
  long node_limit_nds;
  long depth_limit_ply;
  long mate_in_ply;
} bot_term_cond_t;

typedef struct match_state_t {
  // time control info
  size_t wtime;
  size_t btime;
  size_t winc;
  size_t binc;
} match_state_t;

typedef struct bot_t {
  int n_threads;  // number of threads to use
  struct worker_t** workers;

  struct bot_settings_t settings;

  enum {
    SEARCHMODE_INACTIVE,
    SEARCHMODE_REGULAR,
    SEARCHMODE_PONDER
  } search_mode;  // inactive=0, regular=1, ponder=2

  // state
  atomic_bool running, abort;
  struct timespec start_time;
  atomic_intmax_t duration_ms;
  atomic_intmax_t nodes_searched;
  atomic_intmax_t depth_searched;

  // termination conditions
  struct bot_term_cond_t stop_cond;

  // match info
  struct match_state_t match_info;

  // position
  chess_state_t root_position;

  // shared structures
  table_t transpostion_table;
} bot_t;

//
// basic single threaded worker type
typedef struct worker_t {
  // bot api should only look at this bit
  int id;
  pthread_t handle;
  int terminated;
  bot_t* bot;

  chess_state_t position;
  int move_count;
  score_cp_t scores[256];
  move_t moves[256];
  compact_move_t killer_moves[64][MAX_KILLER_MOVES];
  int root_ply;
  int16_t history_heuristic[BUTTERFLY_BOARD_SIZE];
  int16_t butterfly_heuristic[BUTTERFLY_BOARD_SIZE];

} worker_t;

/**
 * Struct to store the search state
 */
typedef struct search_t {
  chess_state_t position;

  // root node info
  int root_move_count;
  score_cp_t root_move_scores[256];
  move_t root_moves[256];

  table_t * transpo_table;
  // table_t * pv_table; // use a seperate table just for pv nodes?!?
  compact_move_t killer_move_table[64][MAX_KILLER_MOVES];

  int root_ply;
} search_t;

#define ALIAS_SEARCH_STATE

/*
typedef struct {
  move_t move, refutation;
  score_cp_t score;
  int depth_searched;
} root_move_t;
*/

// initialises the bot with settings,
// if settings is null it uses default settings
int bot_init(bot_t* bot, bot_settings_t* settings);

// loads the fen string into position
int bot_load_fen(bot_t* bot, const char* postext);

// loads the moves into position
int bot_load_moves(bot_t* bot, const char* movetext);

// starts the bot
int bot_start(bot_t* bot);

// converts the ponder search into regular searching
int bot_ponder_hit(bot_t* bot);

// cancels the search
int bot_stop(bot_t* bot);

// releases bot resources
int bot_release(bot_t* bot);

// called when the bot completes a search or the search is cancelled
void bot_on_stop(bot_t* bot);

int bot_is_running(bot_t* bot);
int bot_wait(bot_t* bot);

#endif