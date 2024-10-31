#include "../include/search.h"

#include "../include/bot.h"
#include "../include/eval.h"
#include "../include/see.h"

int rootSearch(worker_t* worker, score_cp_t alpha, score_cp_t beta, int depth) {
  // aliasing thread data
  chess_state_t* position = &worker->position;
  atomic_fetch_add(&worker->bot->nodes_searched, 1);

  move_t moves[256];
  size_t move_count = generate_legal_moves(position, moves);

  if (move_count == 0) {
    return 1;
  }

  make_move(position, moves[0]);
  score_cp_t first_move_score = -abSearch(worker, -beta, -alpha, depth - 1);
  unmake_move(position);

  worker->best_score = first_move_score;
  worker->best_move = moves[0];

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
    // printf("%d %d\n", alpha, score);
    unmake_move(position);

    if (score >= beta) {
      printf("FAILED HIGH\n");
      return 1;
    }

    if (score > alpha) {
      alpha = score;
    }
    if (score > worker->best_score) {
      worker->best_score = score;
      worker->best_move = moves[i];
    }
  }

  // for (int i = 0; i < move_count; i++) {
  //   char buffer[8];
  //   write_long_algebraic_notation(buffer, sizeof(buffer) , worker->moves[i]);
  //   printf("%s:%03d,", buffer,worker->scores[i]);
  // }
  //   printf("\n");

  return 0;
}

score_cp_t abSearch(worker_t* worker, score_cp_t alpha, score_cp_t beta,
                    int depth) {
  // aliasing thread data
  chess_state_t* position = &worker->position;
  //table_t* table = &worker->bot->transpostion_table;

  atomic_fetch_add(&worker->bot->nodes_searched, 1);

  if (depth <= 0) {
    return eval(position);//qSearch(worker, alpha, beta, depth);
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

  //entry_t tt_entry = tt_get(table, position->zobrist);
  //if (tt_entry) {
  //  if (entry_depth(tt_entry) >= depth) {
  //    return entry_score(tt_entry);
  //  }
  //}

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
      //tt_store(table, position->zobrist, TT_UPPER, moves[i], score, depth, 0);
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
  //enum tt_entry_type entry_type = (best_score > alpha) ? TT_EXACT : TT_LOWER;
  //tt_store(table, position->zobrist, entry_type, best_move, best_score, depth, 0);
  return best_score;
}
/*
score_cp_t qSearch(worker_t* worker, score_cp_t alpha, score_cp_t beta,
                   int depth) {
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
    if (!is_capture(moves[i]) && !is_promotion(moves[i]) &&
        static_exchange_evaluation(position, moves[i]) >= 0) {
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
}*/