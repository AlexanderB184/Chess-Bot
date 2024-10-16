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
  (void)bot;
  thread->terminated = 0;
  thread->depth_searched = 0;
  while (!thread_terminated(thread)) {
    rootSearch(thread, MIN_SCORE, MAX_SCORE, thread->depth_searched++);
  }
  thread->terminated = 1;
  atomic_store(&bot->duration_ms, time_passed(thread));
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
  return write_algebraic_notation(buffer, size, &best_thread->position, best_move);
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

int bot_load_position(bot_t* bot, char* fen, char* movetext) {
  thread_data_t* main_thread = bot->threads[0];
  chess_state_t* root_position = &main_thread->position;
  load_position(root_position, fen);
  while (*movetext) {
    move_t move;
    long out = read_algebraic_notation(movetext, strlen(movetext), root_position, &move);
    if (out == -1) {
      release_position(root_position);
      return -1;
    }
    movetext += out;
    movetext += skip_whitespace(movetext);
    make_move(root_position, move);
  }
  if (main_thread->moves) free(main_thread->moves);
  if (main_thread->root_moves) free(main_thread->root_moves);
  main_thread->moves = malloc(sizeof(move_t) * 256);
  main_thread->move_count = generate_legal_moves(root_position, main_thread->moves);
  main_thread->moves = realloc(main_thread->moves, sizeof(move_t) * main_thread->move_count);
  main_thread->root_moves = malloc(sizeof(root_move_t) * main_thread->move_count);
  for (int i = 1; i < bot->nthreads; i++) {
    thread_data_t* thread = bot->threads[i];
    if (thread->moves) free(thread->moves);
    if (thread->root_moves) free(thread->root_moves);
    copy_position(&thread->position, root_position);
    thread->move_count = main_thread->move_count;
    thread->moves = memcpy(
      malloc(sizeof(move_t) * main_thread->move_count),
      main_thread->moves,
      sizeof(move_t) * main_thread->move_count
    );
    thread->root_moves = malloc(sizeof(root_move_t) * main_thread->move_count);
  }
  return 0;
}

int bot_update_position(bot_t* bot, char* movetext) {
  thread_data_t* main_thread = bot->threads[0];
  chess_state_t* root_position = &main_thread->position;
  while (*movetext) {
    move_t move;
    long out = read_algebraic_notation(movetext, strlen(movetext)+1, root_position, &move);
    if (out == -1) {
      release_position(root_position);
      return -1;
    }
    movetext += out;
    movetext += skip_whitespace(movetext);
    make_move(root_position, move);
  }
  // move validation goes here
  if (is_gameover(root_position)) return -2;
  if (main_thread->moves) free(main_thread->moves);
  if (main_thread->root_moves) free(main_thread->root_moves);
  main_thread->moves = malloc(sizeof(move_t) * 256);
  main_thread->move_count = generate_legal_moves(root_position, main_thread->moves);
  main_thread->moves = realloc(main_thread->moves, sizeof(move_t) * main_thread->move_count);
  main_thread->root_moves = malloc(sizeof(root_move_t) * main_thread->move_count);
  for (int i = 1; i < bot->nthreads; i++) {
    thread_data_t* thread = bot->threads[i];
    copy_position(&thread->position, &main_thread->position);
    if (thread->moves) free(thread->moves);
    if (thread->root_moves) free(thread->root_moves);
    thread->move_count = main_thread->move_count;
    thread->moves = memcpy(
      malloc(sizeof(move_t) * main_thread->move_count),
      main_thread->moves,
      sizeof(move_t) * main_thread->move_count
    );
    thread->root_moves = malloc(sizeof(root_move_t) * main_thread->move_count);

  }
  return 0;
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