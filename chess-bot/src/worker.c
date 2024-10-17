#include "../include/worker.h"

#include "../include/bot.h"
#include "../include/search.h"

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 0
#endif

void bot_on_stop(bot_t* bot) {
    for (int id = 1; id < bot->n_threads; id++) {
        pthread_join(bot->workers[id]->handle, NULL);
    }
    log_info(bot);
    bestmove(bot->workers[0]->best_move, null_move);
    atomic_store(&bot->running, 0);
}

void* worker_start(void* arg) {
  worker_t* worker = (worker_t*)arg;
  bot_t* bot = worker->bot;

  copy_position(&worker->position, &bot->root_position);
  worker->move_count = generate_legal_moves(&worker->position, worker->moves);

  worker->best_move = worker->moves[0];
  worker->best_score = MIN_SCORE;

  while (!stop(worker)) {
    rootSearch(worker, MIN_SCORE, MAX_SCORE, bot->depth_searched);
    bot->depth_searched++;
  }

  release_position(&worker->position);
  worker->terminated = 1;

  if (is_main_thread(worker)) {
    bot_on_stop(bot);
  }
  
  return NULL;
}

size_t time_passed(const struct timespec* start_time) {
  struct timespec curr_time;
  clock_gettime(CLOCK_MONOTONIC, &curr_time);
  size_t delta_s = (curr_time.tv_sec - start_time->tv_sec);
  size_t delta_ns = (curr_time.tv_nsec - start_time->tv_nsec);
  size_t duration_ms = delta_s * 1000 + delta_ns / (1000 * 1000);
  return duration_ms;
}

int stop(worker_t* worker) {
  bot_term_cond_t* stop_cond = &worker->bot->stop_cond;
  bot_t* bot = worker->bot;
  if (atomic_load(&bot->abort)) {
    return 1;
  }
  if (stop_cond->time_limit_ms > 0 &&
      time_passed(&bot->start_time) > stop_cond->time_limit_ms) {
    atomic_store(&bot->abort, 1);
    return 1;
  }
  if (stop_cond->node_limit_nds > 0 &&
      atomic_load(&bot->nodes_searched) > stop_cond->node_limit_nds) {
    atomic_store(&bot->abort, 1);
    return 1;
  }
  if (stop_cond->depth_limit_ply > 0 &&
      atomic_load(&bot->depth_searched) > stop_cond->depth_limit_ply) {
    atomic_store(&bot->abort, 1);
    return 1;
  }
  return 0;
}

int is_main_thread(const worker_t* worker) {
  return (uintptr_t)worker == (uintptr_t)(worker->bot->workers[0]);
}

void log_info(const bot_t* bot) {
  fprintf(stdout, "info depth %ld nodes %ld time %ld\n",
          atomic_load(&bot->depth_searched), atomic_load(&bot->nodes_searched),
          time_passed(&bot->start_time));
  fflush(stdout);
}

void bestmove(move_t bestmove, move_t ponder) {
  char buffer[8];
  write_long_algebraic_notation(buffer, 8, bestmove);
  fprintf(stdout, "bestmove %s", buffer);
  if (!is_null_move(ponder)) {
    write_long_algebraic_notation(buffer, 8, ponder);
    fprintf(stdout, " ponder %s", buffer);
  }
  fprintf(stdout, "\n");
  fflush(stdout);
}