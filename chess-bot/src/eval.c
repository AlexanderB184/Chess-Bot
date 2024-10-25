#include "../include/eval.h"

score_cp_t eval(const chess_state_t* position) {
  /*
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
  */

  // gameover states handled by qSearch

  score_cp_t score =
      +position->friendly_pieces->pawn_count * PAWN_VALUE -
      position->enemy_pieces->pawn_count * PAWN_VALUE +
      position->friendly_pieces->knight_count * KNIGHT_VALUE -
      position->enemy_pieces->knight_count * KNIGHT_VALUE +
      position->friendly_pieces->light_bishop_count * BISHOP_VALUE -
      position->enemy_pieces->light_bishop_count * BISHOP_VALUE +
      position->friendly_pieces->dark_bishop_count * BISHOP_VALUE -
      position->enemy_pieces->dark_bishop_count * BISHOP_VALUE +
      position->friendly_pieces->rook_count * ROOK_VALUE -
      position->enemy_pieces->rook_count * ROOK_VALUE +
      position->friendly_pieces->queen_count * QUEEN_VALUE -
      position->enemy_pieces->queen_count * QUEEN_VALUE;

  return score;
}

score_cp_t piece_value(sq0x88_t sq, piece_t p) {
  switch (p & PIECE_MASK) {
    case PAWN: return PAWN_VALUE;
    case KNIGHT: return KNIGHT_VALUE;
    case BISHOP: return BISHOP_VALUE;
    case ROOK: return ROOK_VALUE;
    case QUEEN: return QUEEN_VALUE;
    case KING: return KING_VALUE;
    default: return 0;
  }
}

int is_repetition(const chess_state_t* chess_state, int ply_of_root) {
  if (chess_state->ply_counter - chess_state->ply_of_last_irreversible_move <
      3) {
    return 0;
  }
  zobrist_t current_zobrist = chess_state->zobrist;
  int repetitions = 1;
  for (int i = chess_state->ply_counter - 2;
       i >= chess_state->ply_of_last_irreversible_move; i -= 2) {
    if (chess_state->ply_stack[i].zobrist == current_zobrist) {
      if (i >= ply_of_root) {
        return 1;
      }
      repetitions++;
    }
  }
  return repetitions >= 3;
}