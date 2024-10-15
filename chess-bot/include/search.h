#ifndef SEARCH_H
#define SEARCH_H

#include "bot-types.h"


move_t   rootSearch(thread_data_t* thread, score_cp_t alpha, score_cp_t beta, int depth);
score_cp_t abSearch(thread_data_t* thread, score_cp_t alpha, score_cp_t beta, int depth);
score_cp_t  qSearch(thread_data_t* thread, score_cp_t alpha, score_cp_t beta, int depth);

#endif