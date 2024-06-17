#include "minmax.h"
#include "quiescence.h"
#include "static_eval.h"
#include "transposition_table.h"

#define return_evaluation(evaluation)                            \
  transposition_table_insert(arena, table, board, (evaluation)); \
  return (evaluation);

double quick_eval_move(const Board* board, Move move) {
  double evaluation = 0;
  if (is_capture(board, move)) {
    evaluation += 6.0;
  }
  if (is_promotion(board, move)) {
    evaluation += 9.0;
  }
  Board cloned_board = copy_make_move(board, move);
  if (is_check(&cloned_board)) {
    evaluation += 5.0;
  }
  evaluation += -(static_evaluation(board) + static_evaluation(&cloned_board));
  return evaluation;
}

Slice(Move) sort_moves_by_heuristic(const Board* board, Slice(Move) moves,
                                    double(heuristic)(const Board*, Move)) {
  double heuristic_values[256];
  for (size_t i = 0; i < moves.length; i++) {
    heuristic_values[i] = heuristic(board, moves.buffer[i]);
  }
  for (int i = 1; i < moves.length; i++) {
    for (int j = 0; j < i; j++) {
      if (heuristic_values[j] < heuristic_values[j + 1]) {
        Move t = moves.buffer[j];
        moves.buffer[j] = moves.buffer[j + 1];
        moves.buffer[j + 1] = t;
        double t_h = heuristic_values[j];
        heuristic_values[j] = heuristic_values[j + 1];
        heuristic_values[j + 1] = t_h;
      }
    }
  }
  return moves;
}

double alpha_beta(Arena* arena, transposition_table_t* table,
                  const Board* board, double alpha, double beta, size_t depth) {
  transposition_table_item_t* node = transposition_table_lookup(table, board);
  if (node) {
    return node->evaluation;
  }
  if (depth == 0) {
    return quiescence_search(arena, table, board, alpha, beta);
  }

  Move move_buffer[256];
  Slice(Move) buffer = (Slice(Move)){move_buffer, lengthof(move_buffer)};
  Slice(Move) moves = generate_legal_moves(buffer, board);
  if (moves.length == 0) {
    if (is_check(board)) {
      return_evaluation(-200.0);
    } else {
      return_evaluation(0.0);
    }
  }
  moves = sort_moves_by_heuristic(board, moves, quick_eval_move);
  for (size_t i = 0; i < moves.length; i++) {
    Move curr_move = moves.buffer[i];
    Board cloned_board = *board;
    make_move(&cloned_board, curr_move);
    double eval =
        -alpha_beta(arena, table, &cloned_board, -beta, -alpha, depth - 1);
    if (eval >= beta) {
      return_evaluation(eval);
    }
    if (eval > alpha) {
      alpha = eval;
    }
  }
  return_evaluation(alpha);
}

Move best_move_ab(const Board* board) {
  Arena arena = {malloc(100 * 1000 * 1000), 0};
  transposition_table_t table;
  transposition_table_init(&arena, &table, 1009);
  Move move_buffer[256];
  Slice(Move) buffer = (Slice(Move)){move_buffer, lengthof(move_buffer)};
  Slice(Move) moves = generate_legal_moves(buffer, board);
  Move best_move = (Move){0};
  double best_eval = -1000000;
  for (size_t i = 0; i < moves.length; i++) {
    Move curr_move = moves.buffer[i];
    Board board_after_move = *board;
    make_move(&board_after_move, curr_move);
    double eval =
        -alpha_beta(&arena, &table, &board_after_move, -1000000, -best_eval, 3);
    if (eval > best_eval) {
      best_eval = eval;
      best_move = curr_move;
    }
  }
  arena_release(&arena);
  return best_move;
}