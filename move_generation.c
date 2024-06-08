#include "move_generation.h"

#include <assert.h>

#include "move_validation.h"
#include "string.h"

Slice_impl(Move);


// TODO: improve this so it isn't O(n^4)
Slice(Move) generate_psuedo_legal_moves(Arena* arena, const Board* board) {
  Slice(Move) moves = make_slice(Move, arena);
  for (size_t i = 0; i < 8; i++) {
    for (size_t j = 0; j < 8; j++) {
      for (size_t n = 0; n < 8; n++) {
        for (size_t m = 0; m < 8; m++) {
          Move move = (Move){i, j, n, m};
          if (board->pieces[i][j].colour ==
                  board->turn /*only needed since is_psuedo_legal_move doesn't
                                 check whose turn it is, this will change when
                                 can_be_attacked is update not to rely on it*/
              && is_psuedo_legal_move(board, move)) {
            if (board->pieces[i][j].type == PAWN && (n == 0 || n == 7)) {
              move.promote_to = QUEEN;
              push_slice(Move, arena, moves, move);
              move.promote_to = ROOK;
              push_slice(Move, arena, moves, move);
              move.promote_to = BISHOP;
              push_slice(Move, arena, moves, move);
              move.promote_to = KNIGHT;
              push_slice(Move, arena, moves, move);
            } else {
              push_slice(Move, arena, moves, move);
            }
          }
        }
      }
    }
  }
  return moves;
}
Slice(Move) generate_legal_moves(Arena* arena, const Board* board) {
  Slice(Move) moves = generate_psuedo_legal_moves(arena, board);
  
  for (size_t i = 0; i < moves.length; i++) {
    if (is_check_after_move(board, moves.buffer[i])) {
      moves.buffer[i] = moves.buffer[--moves.length];
      i--;
    }
  }
  return moves;
}