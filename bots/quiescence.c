#include "quiescence.h"

#include "debug.h"
#include "static_eval.h"
#include "transposition_table.h"

dbg_header();

#define return_evaluation(evaluation)                            \
  transposition_table_insert(arena, table, board, (evaluation)); \
  return (evaluation);

int get_smallest_attacker(const Board* board, int target, Piece_Colour colour) {
  double smallest_value = 1000;
  int smallest_attacker = -1;
  for (int attacker = 0; attacker < 64; attacker++) {
    if (board_get_piece(board, attacker).colour != colour) {
      continue;
    }
    double value = static_piece_value(board_get_piece(board, attacker).type);
    if (value < smallest_value &&
        is_legal_move(board, (Move){attacker, target})) {
      smallest_value = value;
      smallest_attacker = attacker;
    }
  }
  return smallest_attacker;
}

double static_exchange_evaluation(const Board* board, int target,
                                  Piece_Colour colour) {
  double value = 0.0;

  int smallest_attacker = get_smallest_attacker(board, target, colour);
  if (smallest_attacker != -1) {
    double just_captured_value =
        static_piece_value(board_get_piece(board, target).type);
    Move move = (Move){smallest_attacker, target};
    Board cloned_board = copy_make_move(board, move);
    value = max(0, just_captured_value - static_exchange_evaluation(
                                             &cloned_board, target, -colour));
  }
  return value;
}

double static_exchange_capture_evaluation(const Board* board, Move capture) {
  double value = 0;
  Board board_after_move = copy_make_move(board, capture);
  double just_captured_value = board_get_piece(board, capture.to).type;
  value = just_captured_value -
          static_exchange_evaluation(&board_after_move, capture.to,
                                     board_after_move.turn);
  return value;
}

double quiescence_search(Arena* arena, transposition_table_t* table,
                         const Board* board, double alpha, double beta) {
  dbg_sb();
  transposition_table_item_t* node = transposition_table_lookup(table, board);
  if (node) {
    return node->evaluation;
  }
  // special case: is check, search all check evasions
  if (is_check(board)) {
    Move move_buffer[256];
    Slice(Move) buffer = (Slice(Move)){move_buffer, 256};
    Slice(Move) evasions = generate_legal_moves(buffer, board);
    double best_score = -10000;
    for (size_t i = 0; i < evasions.length; i++) {
      Board board_after_move = copy_make_move(board, evasions.buffer[i]);
      double score =
          -quiescence_search(arena, table, &board_after_move, -beta, -alpha);
      if (score >= beta) {
        dbg_bfh();
        return_evaluation(beta);
      }
      if (score > best_score) {
        best_score = score;
        if (score > alpha) {
          alpha = score;
        }
      }
    }
    return_evaluation(alpha);
  }

  double stand_pat = static_evaluation(board);

  if (stand_pat >= beta) {
    dbg_bfh();
    return_evaluation(beta);
  }

  if (alpha < stand_pat) {
    alpha = stand_pat;
  }

  // double start_alpha = alpha;
  Move move_buffer[256];
  Slice(Move) buffer = (Slice(Move)){move_buffer, 256};
  Slice(Move) moves = generate_legal_moves(buffer, board);
  double best_score = -10000;
  for (size_t i = 0; i < moves.length; i++) {
    if (!is_capture(board, moves.buffer[i]) &&
        !is_promotion(board, moves.buffer[i])) {
      continue;
    }
    if (static_exchange_capture_evaluation(board, moves.buffer[i]) < 0.0) {
      continue;
    }
    Board board_after_move = copy_make_move(board, moves.buffer[i]);
    double score =
        -quiescence_search(arena, table, &board_after_move, -beta, -alpha);
    if (score >= beta) {
      dbg_bfh();
      return_evaluation(beta);
    }
    if (score > best_score) {
      best_score = score;
      if (score > alpha) {
        alpha = score;
      }
    }
  }
  // search checking moves
  // if (alpha == start_alpha) {
  //  for (size_t i = 0; i < moves.length; i++) {
  //    Board cloned_board = copy_make_move(board, moves.buffer[i]);
  //    if (is_check(&cloned_board)) {
  // }
  // }
  // }
  return_evaluation(alpha);
}