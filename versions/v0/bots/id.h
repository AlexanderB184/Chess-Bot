#include "../chess.h"
#include "transposition_table.h"
#include <time.h>

void init_id();

Move best_move_id(const Board* board, size_t time_limit);

Move best_move_id_new(const Board* board, size_t time_limit);


extern struct timespec start_time;

void init_clock();

size_t get_time_passed();


extern Arena turn_arena;  // arena with a lifetime of just the current turn;

extern transposition_table_t
    transposition_table;  // transposition table, is reset each turn