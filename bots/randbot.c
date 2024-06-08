#include "randbot.h"

Move rand_move(const Board* board) {
  Scratch_Arena scratch = get_scratch(NULL, 0);
  Slice(Move) moves = generate_legal_moves(scratch.arena, board);
  if (moves.length == 0) {
    release_scratch(scratch);
  return (Move){0};
  }
  Move move = moves.buffer[rand() % (moves.length)];
  release_scratch(scratch);
  return move;
}