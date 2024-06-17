#include "perft.h"

size_t perft(const Board* board, int depth) {
  if (depth == 0) {
    return 1ull;
  }
  size_t count = 0ull;
  Move move_buffer[256];
  Slice(Move) buffer = {move_buffer, 256};
  Slice(Move) moves = generate_legal_moves(buffer, board);
  if (depth == 1) {
    return moves.length;
  }
  for (size_t i = 0; i < moves.length; i++) {
    Board cloned_board = *board;
    make_move(&cloned_board, moves.buffer[i]);
    count += perft(&cloned_board, depth - 1);
  }
  return count;
}