#ifndef WORKER_H
#define WORKER_H

#include "bot.h"

// begins search
void* worker_start(void*);

// checks if this thread is the main thread, i.e. the first thread in the thread list
int is_main_thread(const worker_t*);

// checks if termination condition is met or if the bots abort flag is set
int stop(worker_t*);

// logs info to gui
void log_info();

// returns correctly formatted bestmove to gui
void bestmove(move_t bestmove, move_t ponder);

#endif