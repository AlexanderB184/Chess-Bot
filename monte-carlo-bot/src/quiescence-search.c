#include "../include/quiescence-search.h"

#include "../include/debug.h"
#include "../include/move-order.h"
#include "../include/static-evaluation.h"
//#include "../include/transpo-table.h"

debug_info_t debug_info;

//table_t transpo_table;

/*
int transposition_cutoff_policy(node_t node, score_centipawn_t alpha,
                                score_centipawn_t beta, int depth,
                                int max_depth) {
  int node_type = node_get_type(node);
  score_centipawn_t node_score = node_get_score(node);

  return (node_get_depth_searched(node) >= max_depth - depth) &&
         ((node_type == CUT_NODE && node_score >= beta) ||
          (node_type == ALL_NODE && node_score <= alpha) ||
          node_type == PV_NODE);
}*/

score_centipawn_t check_extension_search(chess_state_t* chess_state,
                                         score_centipawn_t alpha,
                                         score_centipawn_t beta, int depth,
                                         int max_depth) {
  DEBUG_INC_CHECK_EXTENSION_NODES();
  DEBUG_INC_TOTAL_NODES_SEARCHED();

  if (is_draw(chess_state)) {
    return DRAW_SCORE_CENTIPAWNS;
  }

  move_t hash_move = null_move;
/*

  node_index_t index =
      search_or_insert_table(&transpo_table, chess_state->zobrist);

  node_t node = get_node(&transpo_table, index);

  if (!is_null_node(node)) {
    if (transposition_cutoff_policy(node, alpha, beta, depth, max_depth)) {
      DEBUG_INC_TRANSPO_CUTOFFS();
      return node_get_score(node);
    }
    hash_move = node_get_hash_move(node);
  }*/

  move_t moves[256];
  size_t move_count = generate_check_evasions(chess_state, moves);
  generate_priorities(chess_state, moves, move_count, hash_move);
  move_t move = next_move(chess_state, moves, &move_count);
  //move_t best_move = move;

  if (is_null_move(move)) {
    return CHECKMATE_SCORE_CENTIPAWNS;
  }

  make_move(chess_state, move);
  score_centipawn_t best_score =
      -quiescence_search(chess_state, -beta, -alpha, depth + 1, max_depth);
  unmake_move(chess_state);

  if (best_score >= beta) {
    DEBUG_INC_BETA_CUTOFFS();
    //node_t new_node = make_node(move, best_score, CUT_NODE, max_depth, depth);
    //update_table(&transpo_table, index, chess_state->zobrist, new_node);
    return best_score;
  }

  if (best_score > alpha) {
    alpha = best_score;
  }

  while (!is_null_move(move = next_move(chess_state, moves, &move_count))) {
    make_move(chess_state, move);
    score_centipawn_t score =
        -quiescence_search(chess_state, -beta, -alpha, depth + 1, max_depth);
    unmake_move(chess_state);

    if (score >= beta) {
      DEBUG_INC_BETA_CUTOFFS();
      //node_t new_node = make_node(move, score, CUT_NODE, max_depth, depth);
      //update_table(&transpo_table, index, chess_state->zobrist, new_node);
      return score;
    }
    if (score > best_score) {
      best_score = score;
      //best_move = move;
      if (score > alpha) {
        alpha = score;
      }
    }
  }
  //node_t new_node =
  //    make_node(best_move, best_score, ALL_NODE, max_depth, depth);
  //update_table(&transpo_table, index, chess_state->zobrist, new_node);
  return best_score;
}

// TODO: incorporate transposition table
score_centipawn_t quiescence_search(chess_state_t* chess_state,
                                    score_centipawn_t alpha,
                                    score_centipawn_t beta, int depth,
                                    int max_depth) {
  if (is_draw(chess_state)) {
    return DRAW_SCORE_CENTIPAWNS;
  }

  if (is_check(chess_state)) {
    return check_extension_search(chess_state, alpha, beta, depth, max_depth);
  }

  DEBUG_INC_TOTAL_NODES_SEARCHED();
  DEBUG_INC_QUIESCENCE_NODES();

  score_centipawn_t stand_pat = static_evaluation(chess_state);

  if (stand_pat > beta) {
    DEBUG_INC_BETA_CUTOFFS();
    return stand_pat;
  }
  // if swing of a queen is not enough to improve position we do not need to
  // evaluate
  if (stand_pat < alpha - 950) {
    DEBUG_INC_DELTA_CUTOFFS();
    return stand_pat;
  }

  if (stand_pat > alpha) {
    alpha = stand_pat;
  }

  move_t hash_move = null_move;
/*
  node_index_t index =
      search_or_insert_table(&transpo_table, chess_state->zobrist);

  node_t node = get_node(&transpo_table, index);

  if (!is_null_node(node)) {
    if (transposition_cutoff_policy(node, alpha, beta, depth, max_depth)) {
      DEBUG_INC_TRANSPO_CUTOFFS();
      return node_get_score(node);
    }
    hash_move = node_get_hash_move(node);
  }

  if (!is_null_move(hash_move) && !is_capture(hash_move)) {
    return stand_pat;  //? best response is not a capture we can stop quiescence
                       // search??!?!?!
  }*/

  score_centipawn_t best_score = stand_pat;

  move_t moves[256];
  size_t move_count = generate_moves(chess_state, moves);
  // could be optimised with specialised get priority function
  generate_priorities(chess_state, moves, move_count, hash_move);

  move_t move = next_move(chess_state, moves, &move_count);
  //move_t best_move = move;

  if (is_null_move(move)) {
    return STALEMATE_SCORE_CENTIPAWNS;
  }

  do {
    // only evaluate see >= 0
    if (get_priority(move) < PRIORITY_NEUTRAL_CAPTURE_SEQUENCE) {
      break;
    }
    // delta pruning
    if (stand_pat + board_value(chess_state, get_to(move)) < alpha - 200) {
      DEBUG_INC_DELTA_CUTOFFS();
      continue;
    }

    make_move(chess_state, move);
    score_centipawn_t score =
        -quiescence_search(chess_state, -beta, -alpha, depth + 1, max_depth);
    unmake_move(chess_state);

    if (score >= beta) {
      DEBUG_INC_BETA_CUTOFFS();
      //node_t new_node = make_node(move, score, CUT_NODE, max_depth, depth);
      //update_table(&transpo_table, index, chess_state->zobrist, new_node);
      return score;
    }

    if (score > best_score) {
      best_score = score;
      //best_move = move;

      if (score > alpha) {
        alpha = score;
      }
    }
  } while (!is_null_move(move = next_move(chess_state, moves, &move_count)));
  // search checks??
  // if (best_score <= stand_pat) {
  //    ...
  // }

  //node_t new_node =
  //    make_node(best_move, best_score, CUT_NODE, max_depth, depth);
  //update_table(&transpo_table, index, chess_state->zobrist, new_node);
  return best_score;
}