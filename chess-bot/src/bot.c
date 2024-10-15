#include "../include/bot.h"
#include "../include/search.h"

#include <stdlib.h>
#include <pthread.h>
#include <time.h>


#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 0
#endif

size_t time_passed(thread_data_t* thread) {
  struct timespec current_time;
  clock_gettime(CLOCK_MONOTONIC, &current_time);
  size_t elapsed_time = (current_time.tv_sec  - thread->bot->start_time.start_time.tv_sec) * 1000;
  elapsed_time +=       (current_time.tv_nsec - thread->bot->start_time.tv_nsec          ) / 1000000;
  return elapsed_time;
}

int bot_run(void* arg) {
    thread_data_t* thread = (thread_data_t*)arg;

    while (thread->abort) {
        
    }

    for (int depth = 0; !thread_terminate(thread); depth++) {
        rootSearch(thread, MIN_SCORE, MAX_SCORE, depth);
    }
}

int thread_terminate(thread_data_t* thread) {
    bot_term_cond_t* stop_cond = &thread->bot->stop_cond;
    if (thread->abort) return 1;
    if (stop_cond->time_limit_ms > 0 && time_passed(thread) >= stop_cond->time_limit_ms) return 1;
    if (stop_cond->node_limit_nds > 0 && atomic_load(&thread->bot->nodes_searched) >= stop_cond->time_limit_ms) return 1;
}

move_t bot_move(bot_t* bot) {
  thread_data_t* best_thread = bot->threads[0];
  size_t move_count = best_thread->move_count;
  root_move_t* root_move_data = best_thread->root_moves;
  move_t* moves = best_thread->moves;
  move_t best_move = moves[0];
  score_cp_t best_score = root_move_data[0].score;
  for (int i = 1; i < best_thread->move_count; i++) {
    if (root_move_data[i].score > best_score) {
      best_score = root_move_data[i].score;
      best_move = moves[i];
    }
  }
  return best_move;
}

int bot_init(bot_t* bot, bot_settings_t* settings) {
  bot->abort = 1;
  if (settings) {
    bot->nthreads = settings->nthreads;
  } else {
    bot->nthreads = 1;
  }
  bot->threads = calloc(bot->nthreads, sizeof(*bot->threads));
  for (int i = 0; i < bot->nthreads; i++) {
    bot->threads[i] = calloc(1, sizeof(thread_data_t));
    bot->threads[i]->abort = &bot->abort;
  }
}

int bot_load_position(bot_t* bot, char* fen, char** movetext) {
  for (int i = 0; i < bot->nthreads; i++) {
    load_position(&bot->threads[i]->position, fen);
    while (movetext) {
      move_t move = read_algebraic_notation(movetext, 32, &bot->threads[i]->position);
      make_move(&bot->threads[i]->position, move);
      movetext++;
    }
    bot->threads[i]->moves = malloc(sizeof(move_t) * 256);
    bot->threads[i]->move_count = generate_legal_moves(&bot->threads[i]->position, bot->threads[i]->moves);
    bot->threads[i]->moves = realloc(bot->threads[i]->moves, sizeof(move_t) * bot->threads[i]->move_count);
    bot->threads[i]->root_moves = malloc(sizeof(root_move_t) * bot->threads[i]->move_count);
  }
}

int bot_start(bot_t* bot, bot_term_cond_t* stop_cond) {
    clock_gettime(CLOCK_MONOTONIC, &bot->start_time);

    bot->stop_cond = *stop_cond;
    bot->nodes_searched = 0;
    bot->abort = 0;

    return 0;

}

int bot_stop(bot_t* bot) {
    bot->abort = 1;
    // wait for threads to terminate?
    return 0;
}

int bot_release(bot_t* bot) {
    for (int i = 0; i < bot->nthreads; i++) {
        clear_position(&bot->threads[i]->position);
        free(bot->threads[i]->moves);
        free(bot->threads[i]->root_moves);
        free(bot->threads[i]);
    }
    free(bot->threads);
    return 0;

}

