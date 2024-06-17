#ifndef QUIESCENCE_SEARCH_H
#define QUIESCENCE_SEARCH_H

#include "../chess.h"
#include "transposition_table.h"

double quiescence_search(Arena* arena, transposition_table_t* table,
                         const Board* board, double alpha, double beta);

#endif