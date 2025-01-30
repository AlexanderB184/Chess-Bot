#ifndef SEARCH_H
#define SEARCH_H

#include "bot.h"
#include "worker.h"


int      rootSearch(worker_t* worker, score_cp_t alpha, score_cp_t beta, int depth);
score_cp_t abSearch(worker_t* worker, score_cp_t alpha, score_cp_t beta, int depth);
score_cp_t  qSearch(worker_t* worker, score_cp_t alpha, score_cp_t beta, int depth);

#endif