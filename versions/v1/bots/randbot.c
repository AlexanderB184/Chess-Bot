#include "randbot.h"

Move rand_move(const Board* board) {
 Move move_buffer[256];
  Slice(Move) buffer = {move_buffer, 256};
  Slice(Move) moves = generate_legal_moves(buffer, board);
  if (moves.length == 0) {
  return (Move){0};
  }
  Move move = moves.buffer[rand() % (moves.length)];
  return move;
}