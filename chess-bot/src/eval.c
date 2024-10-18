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
  + position->friendly_pieces->pawn_count         * PAWN_VALUE
  - position->enemy_pieces->pawn_count            * PAWN_VALUE
  + position->friendly_pieces->knight_count       * KNIGHT_VALUE
  - position->enemy_pieces->knight_count          * KNIGHT_VALUE
  + position->friendly_pieces->light_bishop_count * BISHOP_VALUE
  - position->enemy_pieces->light_bishop_count    * BISHOP_VALUE
  + position->friendly_pieces->dark_bishop_count  * BISHOP_VALUE
  - position->enemy_pieces->dark_bishop_count     * BISHOP_VALUE
  + position->friendly_pieces->rook_count         * ROOK_VALUE
  - position->enemy_pieces->rook_count            * ROOK_VALUE
  + position->friendly_pieces->queen_count        * QUEEN_VALUE
  - position->enemy_pieces->queen_count           * QUEEN_VALUE;

  return score;
}