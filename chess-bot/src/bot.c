#include "../include/bot.h"

#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "../include/search.h"

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 0
#endif

size_t time_passed(thread_data_t* thread) {
  struct timespec current_time;
  clock_gettime(CLOCK_MONOTONIC, &current_time);
  size_t elapsed_time =
      (current_time.tv_sec - thread->bot->start_time.tv_sec) * 1000000000;
  elapsed_time +=
      (current_time.tv_nsec - thread->bot->start_time.tv_nsec);
  return elapsed_time;
}

void* bot_run(void* arg) {
  thread_data_t* thread = (thread_data_t*)arg;
  bot_t* bot = thread->bot;
  copy_position(&thread->position, &bot->root_position);
  thread->terminated = 0;
  thread->depth_searched = 0;
  while (!thread_terminated(thread)) {
    rootSearch(thread, MIN_SCORE, MAX_SCORE, thread->depth_searched++);
  }
  if (thread->id == 0) { // main thread
    // wait for all other threads to terminate
    for (int i = 1; i < bot->nthreads; i++) {
      pthread_join(bot->threads[i]->thread_handle, NULL);
    }
    atomic_store(&bot->duration_ms, time_passed(thread));
    // Send final log
    START_LOG(bot);
    LOG_DEPTH(bot);
    LOG_NODES(bot);
    LOG_TIME(bot);
    END_LOG(bot);
    SUBMIT_BEST_MOVE(bot);
  }
  thread->terminated = 1;
  return NULL;
}

int thread_terminated(thread_data_t* thread) {
  bot_term_cond_t* stop_cond = &thread->bot->stop_cond;
  if (atomic_load(&thread->bot->abort)) {
    return 1;
  }
  if (stop_cond->time_limit_ms > 0 &&
      time_passed(thread) > stop_cond->time_limit_ms * 1000000) {
    atomic_store(&thread->bot->abort, 1);
    return 1;
  }
  if (stop_cond->node_limit_nds > 0 &&
      atomic_load(&thread->bot->nodes_searched) > stop_cond->node_limit_nds) {
    atomic_store(&thread->bot->abort, 1);
    return 1;
  }
  if (stop_cond->depth_limit_ply > 0 &&
      atomic_load(&thread->depth_searched) > stop_cond->depth_limit_ply) {
    atomic_store(&thread->bot->abort, 1);
    return 1;
  }
  return 0;
}

long bot_move(bot_t* bot, char* buffer, int size) {
  thread_data_t* best_thread = bot->threads[0];
  size_t move_count = best_thread->move_count;
  root_move_t* root_move_data = best_thread->root_moves;
  move_t* moves = best_thread->moves;
  if (move_count == 0) {
    return -1;
  }
  move_t best_move = moves[0];
  score_cp_t best_score = root_move_data[0].score;
  for (int i = 1; i < move_count; i++) {
    if (root_move_data[i].score > best_score) {
      best_score = root_move_data[i].score;
      best_move = moves[i];
    }
  }
  return write_long_algebraic_notation(buffer, size, best_move);
}

int bot_init(bot_t* bot, bot_settings_t* settings) {
  bot->abort = 1;
  if (settings) {
    bot->nthreads = settings->nthreads;
  } else {
    bot->nthreads = 1;
  }
  bot->threads = calloc(bot->nthreads, sizeof(*bot->threads));
  for (int id = 0; id < bot->nthreads; id++) {
    bot->threads[id] = calloc(1, sizeof(thread_data_t));
    *bot->threads[id] = (thread_data_t){.id = id, .bot = bot};
  }
  return 0;
}

int bot_load_fen(bot_t* bot, char* pos_text) {
  chess_state_t* root_position = &bot->root_position;
  return load_position(root_position, pos_text);
}

int bot_load_moves(bot_t* bot, char* movetext) {
  chess_state_t* root_position = &bot->root_position;
  long bytes_read = 0;
  long bytes_to_read = strlen(movetext);
  while (movetext + bytes_read) {
    move_t move;
    long out = read_long_algebraic_notation(movetext + bytes_read, bytes_to_read - bytes_read, root_position, &move);
    if (out == -1) {
      release_position(root_position);
      return -1;
    }
    bytes_read += out;
    bytes_read += skip_whitespace(movetext + bytes_read);
    make_move(root_position, move);
  }
  return bytes_read;
}

int bot_start(bot_t* bot, bot_term_cond_t* stop_cond) {
  clock_gettime(CLOCK_MONOTONIC, &bot->start_time);
  if (stop_cond) {
    bot->stop_cond = *stop_cond;
  } else {
    // default stop condition
    bot->stop_cond = (bot_term_cond_t){
        .depth_limit_ply = 0,
        .node_limit_nds = 0,
        .time_limit_ms = 100,
    };
  }
  atomic_store(&bot->nodes_searched, 0);
  atomic_store(&bot->abort, 0);
  for (int i = 0; i < bot->nthreads; i++) {

    int r = pthread_create(&bot->threads[i]->thread_handle, NULL, bot_run,
                           bot->threads[i]);
    if (r) {
      bot_stop(bot);
      return -1;
    }
  }

  return 0;
}

int bot_stop(bot_t* bot) {
  atomic_store(&bot->abort, 1);
  // Wait for each thread to finish
  bot_wait(bot);
  return 0;
}

int bot_wait(bot_t* bot) {
  for (int i = 0; i < bot->nthreads; i++) {
    pthread_join(bot->threads[i]->thread_handle, NULL);
  }
  return 0;
}

int bot_is_running(bot_t* bot) {
  for (int i = 0; i < bot->nthreads; i++) {
    if (!bot->threads[i]->terminated) {
      return 1;
    }
  }
  return 0;
}

int bot_release(bot_t* bot) {
  bot_stop(bot);
  for (int i = 0; i < bot->nthreads; i++) {
    release_position(&bot->threads[i]->position);
    free(bot->threads[i]->moves);
    free(bot->threads[i]->root_moves);
    free(bot->threads[i]);
  }
  free(bot->threads);
  return 0;
}

int bot_save_stats(bot_t* bot, FILE* stream) {
  fprintf(stream, "{\"nodes_searched\":%zu,\"duration_ms\":%f},\n", atomic_load(&bot->nodes_searched), (double)atomic_load(&bot->duration_ms) / 1000000);
  return 0;
}