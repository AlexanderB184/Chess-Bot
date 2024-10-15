#include "../include/search.h"
#include "../include/eval.h"
#include "../include/bot.h"

int rootSearch(thread_data_t* thread, score_cp_t alpha, score_cp_t beta, int depth) {
  // aliasing thread data
  chess_state_t* position = &thread->position;
  size_t move_count = thread->move_count;
  move_t* moves = thread->moves;
  root_move_t* root_move_data = thread->root_moves;
  atomic_fetch_add(&thread->bot->nodes_searched, 1);

  if (move_count == 0) {
    return 1;
  }

  move_t best_move = moves[0];

  make_move(position, best_move);
  score_cp_t score = -abSearch(thread, -beta, -alpha, depth - 1);
  unmake_move(position);
  root_move_data[0].score = score;

  if (score > alpha) {
    if (score >= beta) {
        return 1;
    }
    alpha = score;
  }

  for (size_t i = 1; !thread_terminated(thread) && i < move_count; i++) {
    make_move(position, moves[i]);
    score = -abSearch(thread, -beta, -alpha, depth - 1);
    unmake_move(position);
    root_move_data[i].score = score;


    if (score >= beta) {
      return 1;
    }

    if (score > alpha) {
      alpha = score;
    }
    
  }

  return 0;
}

score_cp_t abSearch(thread_data_t* thread, score_cp_t alpha, score_cp_t beta, int depth) {
  // aliasing thread data
  chess_state_t* position = &thread->position;
  atomic_fetch_add(&thread->bot->nodes_searched, 1);

  if (depth <= 0) {
    return eval(position);
  }

  if (is_draw(position)) {
    return DRAW_SCORE_CENTIPAWNS;
  }

  move_t moves[256];
  size_t move_count = generate_legal_moves(position, moves);

  if (move_count == 0) {
    if (is_check(position)) {
      return CHECKMATE_SCORE_CENTIPAWNS - depth;
    } else {
      return STALEMATE_SCORE_CENTIPAWNS;
    }
  }

  move_t best_move = moves[0];

  make_move(position, best_move);
  score_cp_t best_score = -abSearch(thread, -beta, -alpha, depth - 1);
  unmake_move(position);

  if (best_score > alpha) {
    if (best_score >= beta) {
      return best_score;
    }
    alpha = best_score;
  }

  for (size_t i = 1; !thread_terminated(thread) && i < move_count; i++) {
    make_move(position, moves[i]);
    score_cp_t score = -abSearch(thread, -beta, -alpha, depth - 1);
    unmake_move(position);

    if (score >= beta) {
      return score;
    }

    if (score > best_score) {
      best_score = score;
      best_move = moves[i];
      if (score > alpha) {
        alpha = score;
      }
    }
  }

  return best_score;
}