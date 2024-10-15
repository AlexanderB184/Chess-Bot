#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../chess-bot/include/iterative-deepening.h"
#include "../chess-lib/include/chess-lib.h"


int play_chess_match() {
  static int matches_count = 0;
  chess_state_t match_state;
  memset(&match_state, 0, sizeof(chess_state_t));
  int results = 0;
  load_start_position(&match_state);
  while (!is_gameover(&match_state)) {
    move_t move;
    if (is_white_to_move(&match_state)) {
      move = find_best_move(&match_state);
      char buffer[256];
      buffer[write_algebraic_notation(buffer, 256, &match_state, move)] = '\0';
      printf("%s played %s\n", "WHITE", buffer);
    } else {
      move = find_best_move(&match_state);
      char buffer[256];
      buffer[write_algebraic_notation(buffer, 256, &match_state, move)] = '\0';
      buffer[write_algebraic_notation(buffer, 256, &match_state, move)] = '\0';
      printf("%s played %s\n", "BLACK", buffer);
    }
    make_move(&match_state, move);
  }
  if (is_draw(&match_state)) {
    // checks for stalemate, draw by repetition, draw by insufficient material
    // and draw by 50 move rule
    results = 0;
  } else if (is_checkmate(&match_state) && is_white_to_move(&match_state)) {
    // if it is checkmate and it is whites turn that means they are in checkmate
    results = -1;
  } else if (is_checkmate(&match_state) && is_black_to_move(&match_state)) {
    // if it is checkmate and it is blacks turn that means they are in checkmate
    results = 1;
  }
  // clear_position(&match_state);
  printf("completed matches: %d\n", ++matches_count);
  clear_position(&match_state);
  return results;
}

/*
void play_tournament_mult(bot_t *bots, int bot_count,
                          int matches_per_bot_pair) {
  int num_threads = bot_count / 2;
  pthread_t threads[num_threads];
  task_queue_t queue;
  queue.tasks = malloc(sizeof(task_t) * (bot_count * (bot_count - 1) / 2));
  queue.task_index = 0;
  queue.task_count = 0;
  pthread_mutex_init(&queue.mutex, NULL);
  pthread_cond_init(&queue.cond, NULL);

  // Initialize bot locks
  bot_locks = malloc(sizeof(pthread_mutex_t) * bot_count);
  for (int i = 0; i < bot_count; i++) {
    pthread_mutex_init(&bot_locks[i], NULL);
  }

  // Create the tasks
  for (int i = 0; i < bot_count; i++) {
    for (int j = i + 1; j < bot_count; j++) {
      queue.tasks[queue.task_count++] =
          (task_t){&bots[i], &bots[j], matches_per_bot_pair, i, j};
    }
  }

  shuffle_tasks(queue.tasks, queue.task_count);

  // Create the threads
  for (int i = 0; i < num_threads; i++) {
    pthread_create(&threads[i], NULL, worker_function, &queue);
  }

  // Signal the threads to start processing
  pthread_mutex_lock(&queue.mutex);
  pthread_cond_broadcast(&queue.cond);
  pthread_mutex_unlock(&queue.mutex);

  // Wait for all threads to finish
  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }

  // Cleanup
  free(queue.tasks);
  for (int i = 0; i < bot_count; i++) {
    pthread_mutex_destroy(&bot_locks[i]);
  }
  free(bot_locks);
  pthread_mutex_destroy(&queue.mutex);
  pthread_cond_destroy(&queue.cond);
}*/

int main(int argc, char const *argv[]) {
  play_chess_match();
  return 0;
}
