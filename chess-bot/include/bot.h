#ifndef BOT_H
#define BOT_H

#include "../../chess-lib/include/chess-lib.h"
#include "bot-types.h"

int thread_terminate(thread_data_t* thread);

// initialised the bot to a valid state
// intiailised with settings from 'settings'
// if 'settings' is NULL bot is initialised with defaults
int bot_init(bot_t* bot, bot_settings_t* settings);

// loads a position into the bot's internal state
int bot_load_position(bot_t* bot, char* fen, char* movetext);

// starts the bot executing the search, stopping either when one of the
// conditions in 'stop_cond' is met or 'bot_stop' is called.
int bot_start(bot_t* bot, bot_term_cond_t* stop_cond);

// runs the search in ponder mode, stopping if 'bot_stop' is called or if 'ponder_hit' or 'ponder_miss' are called.
// 'ponder_hit' converts the ponder search into a regular search.
// 'ponder_miss' cancels the ponder search and restarts with a regular search.
int bot_ponder(bot_t* bot, move_t move);
int bot_ponder_hit(bot_t* bot);
int bot_ponder_miss(bot_t* bot);

// signals to the bot to stop running as soon as possible
int bot_stop(bot_t* bot);

// releases the resources of the bot
int bot_release(bot_t* bot);

// uses the results from the search to select a move.
move_t bot_move(bot_t* bot);

#endif