#ifndef BOT_H
#define BOT_H

#include "../../chess-lib/include/chess-lib.h"
#include "bot-types.h"

#include <stdio.h>

int thread_terminated(thread_data_t* thread);

// initialised the bot to a valid state
// intiailised with settings from 'settings'
// if 'settings' is NULL bot is initialised with defaults
int bot_init(bot_t* bot, bot_settings_t* settings);

// loads a position into the bot's internal state
int bot_load_position(bot_t* bot, char* position_text);

// plays a move in the current position
int bot_update_position(bot_t* bot, char* movetext);

// starts the bot executing the search, stopping either when one of the
// conditions in 'stop_cond' is met or 'bot_stop' is called.
int bot_start(bot_t* bot, bot_term_cond_t* stop_cond);

// runs the search in ponder mode, stopping if 'bot_stop' is called or if 'ponder_hit' or 'ponder_miss' are called.
// 'ponder_hit' converts the ponder search into a regular search.
// 'ponder_miss' cancels the ponder search and restarts with a regular search.
int bot_ponder(bot_t* bot, char* move);
int bot_ponder_hit(bot_t* bot);
int bot_ponder_miss(bot_t* bot);

// signals to the bot to stop running as soon as possible
int bot_stop(bot_t* bot);

// releases the resources of the bot
int bot_release(bot_t* bot);

// uses the results from the search to select a move.
long bot_move(bot_t* bot, char* buffer, int size);

int bot_wait(bot_t* bot);
int bot_is_running(bot_t* bot);

int bot_save_stats(bot_t* bot, FILE* stream);

#define START_LOG(BOT) fprintf((BOT)->log_stream, "info")
#define LOG_DEPTH(BOT) fprintf((BOT)->log_stream, " depth %zu", (BOT)->threads[0]->depth_searched)
#define LOG_NODES(BOT) fprintf((BOT)->log_stream, " nodes %zu", (BOT)->nodes_searched)
#define LOG_TIME(BOT) fprintf((BOT)->log_stream, " time %zu", (BOT)->duration_ms)
#define END_LOG(BOT) fprintf((BOT)->log_stream, "\n")

#define SUBMIT_BEST_MOVE(BOT)                                 \
  {                                                           \
    char move_buffer[16];                                     \
    bot_move((BOT), move_buffer, sizeof(move_buffer));        \
    fprintf((BOT)->log_stream, "bestmove %s\n", move_buffer); \
  }

#endif