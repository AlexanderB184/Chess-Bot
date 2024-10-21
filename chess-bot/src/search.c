#include "../include/search.h"
#include "../include/eval.h"
#include "../include/bot.h"

int rootSearch(worker_t* worker, score_cp_t alpha, score_cp_t beta, int depth) {
  // aliasing thread data
  chess_state_t* position = &worker->position;
  size_t move_count = worker->move_count;
  move_t* moves = worker->moves;
  atomic_fetch_add(&worker->bot->nodes_searched, 1);

  if (move_count == 0) {
    return 1;
  }

  make_move(position, moves[0]);
  score_cp_t first_move_score = -abSearch(worker, -beta, -alpha, depth - 1);
  unmake_move(position);
  worker->scores[0] = first_move_score;

  if (first_move_score > alpha) {
    if (first_move_score >= beta) {
      printf("FAILED HIGH\n");
        return 1;
    }
    alpha = first_move_score;
  }

  for (size_t i = 1; !stop(worker) && i < move_count; i++) {
    make_move(position, moves[i]);
    
    score_cp_t score = -abSearch(worker, MIN_SCORE, MAX_SCORE, depth - 1);
    //printf("%d %d\n", alpha, score);
    unmake_move(position);
    worker->scores[i] = score;

    if (score >= beta) {
      printf("FAILED HIGH\n");
      return 1;
    }

    if (score > alpha) {
      alpha = score;
    }
  }

  for (int i = 1; i < move_count; i++) {
    for (int j = i; j > 0; j--) {
      if (worker->scores[j - 1] < worker->scores[j]) {
        score_cp_t tscore = worker->scores[j - 1];
        worker->scores[j - 1] = worker->scores[j];
        worker->scores[j] = tscore;

        move_t tmove = worker->moves[j - 1];
        worker->moves[j - 1] = worker->moves[j];
        worker->moves[j] = tmove;
      } else {
        break;
      }
    }
  }
  //for (int i = 0; i < move_count; i++) {
  //  char buffer[8];
  //  write_long_algebraic_notation(buffer, sizeof(buffer) , worker->moves[i]);
  //  printf("%s:%03d,", buffer,worker->scores[i]);
  //}
  //  printf("\n");
    

  return 0;
}

score_cp_t abSearch(worker_t* worker, score_cp_t alpha, score_cp_t beta, int depth) {
  // aliasing thread data
  chess_state_t* position = &worker->position;

  if (depth <= 0 && !is_check(position)) {
    return qSearch(worker, alpha, beta, depth);
  }
  atomic_fetch_add(&worker->bot->nodes_searched, 1);

  if (is_draw_by_50_move_rule(position)
   || is_draw_by_insufficient_material(position)
   || is_repetition(position, worker->root_ply)) {
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
  score_cp_t best_score = -abSearch(worker, -beta, -alpha, depth - 1);
  unmake_move(position);
  
  if (best_score > alpha) {
    if (best_score >= beta) {
      return best_score;
    }
    alpha = best_score;
  }

  for (size_t i = 1; !stop(worker) && i < move_count; i++) {
    make_move(position, moves[i]);
    score_cp_t score = -abSearch(worker, -beta, -alpha, depth - 1);
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

score_cp_t  qSearch(worker_t* worker, score_cp_t alpha, score_cp_t beta, int depth) {
  chess_state_t* position = &worker->position;

  if (is_check(position)) {
    return abSearch(worker, alpha, beta, depth);
  }

  atomic_fetch_add(&worker->bot->nodes_searched, 1);

  if (is_draw(position)) {
    return DRAW_SCORE_CENTIPAWNS;
  }

  move_t moves[256];
  size_t move_count = generate_legal_moves(position, moves);

  if (move_count == 0) {
    if (is_check(position)) {
      return CHECKMATE_SCORE_CENTIPAWNS;
    } else {
      return STALEMATE_SCORE_CENTIPAWNS;
    }
  }

  score_cp_t stand_pat = eval(position);

  if (stand_pat >= beta) {
    return stand_pat;
  }

  if (stand_pat > alpha) {
    alpha = stand_pat;
  }

  score_cp_t best_score = stand_pat;

  for (int i = 0; i < move_count; i++) {
    if (!is_capture(moves[i]) && !is_promotion(moves[i])) {
      continue;
    }

    make_move(position, moves[i]);
    score_cp_t score = -qSearch(worker, -beta, -alpha, depth - 1);
    unmake_move(position);

    if (score >= beta) {
      return score;
    }

    if (score > best_score) {
      best_score = score;
      if (score > alpha) {
        alpha = score;
      }
    }
  }

  return best_score;
}