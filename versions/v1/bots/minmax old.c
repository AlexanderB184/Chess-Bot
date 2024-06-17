#include "minmax.h"


double evaluate2(const Board*, size_t);
double static_eval(const Board*);

Move best_move_old(const Board* board) {
  Scratch_Arena scratch = get_scratch(NULL, 0);
  Slice(Move) moves = generate_legal_moves(scratch.arena, board);
  Move best_move = (Move){0};
  double best_eval = -100000.0f;
  for (size_t i = 0; i < moves.length; i++) {
    Move curr_move = moves.buffer[i];
    Board board_after_move = *board;
    make_move(&board_after_move, curr_move);
    double eval = -evaluate2(&board_after_move,2);
    if (eval > best_eval) {
      best_eval = eval;
      best_move = curr_move;
    }
  }
  release_scratch(scratch);
  return best_move;
}

double static_eval(const Board* board) {
  if (is_stalemate(board)) {
    if (is_check(board)) {
      return -10000.0f;
    } else {
      return -0.1f;
    }
  }
  double evalulation = 0.0f;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      switch (board->pieces[i][j].type) {
        case EMPTY:
          evalulation += 0.0f;
          break;
        case PAWN:
          evalulation += (board->pieces[i][j].colour) * 1.0f;
          break;
        case KNIGHT:
          evalulation += (board->pieces[i][j].colour) * 3.0f;
        case BISHOP:
          evalulation += (board->pieces[i][j].colour) * 3.0f;
        case ROOK:
          evalulation += (board->pieces[i][j].colour) * 5.0f;
        case QUEEN:
          evalulation += (board->pieces[i][j].colour) * 9.0f;
        case KING:
          evalulation += 0.0f;
          break;
        default:
          break;
      }
    }
  }
  return evalulation * board->turn;
}

double evaluate2(const Board* board, size_t depth) {
  if (depth == 0) {
    return static_eval(board);  // static evaluate
  }
  if (board->half_move_count >= 50) {
    return -0.1f;
  }
  Scratch_Arena scratch = get_scratch(NULL, 0);
  Slice(Move) moves = generate_legal_moves(scratch.arena, board);
  if (moves.length == 0) {
    release_scratch(scratch);
    if (is_check(board)) {
      return -10000.0f;
    } else {
      return -0.1f;
    }
  }
  double best_eval = -1000.0f;
  for (size_t i = 0; i < moves.length; i++) {
    Move curr_move = moves.buffer[i];
    Board cloned_board = *board;
    make_move(&cloned_board, curr_move);
    double eval = -evaluate2(&cloned_board, depth - 1);
    if (eval > best_eval) {
      best_eval = eval;
    }
  }
  release_scratch(scratch);
  return best_eval;
}