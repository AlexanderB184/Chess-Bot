#include "../include/butterfly-board.h"

#include <string.h>

butterfly_index_t get_butterfly_index(move_t move) {
  return sq0x88_to_sq8x8(move.from) | (sq0x88_to_sq8x8(move.to) << 6);
}

void inc_butterfly_board(butterfly_board_t* butterfly_board, move_t move) {
  butterfly_index_t index = get_butterfly_index(move);
  if ((*butterfly_board)[index] < 0xFF) {
    (*butterfly_board)[index]++;
  }
}
void clear_butterfly_board(butterfly_board_t* butterfly_board) {
  memset(*butterfly_board, 0, sizeof(butterfly_board_t));
}
uint8_t get_butterfly_board(butterfly_board_t* butterfly_board, move_t move) {
  butterfly_index_t index = get_butterfly_index(move);
  return (*butterfly_board)[index];
}
void set_butterfly_board(butterfly_board_t* butterfly_board, move_t move,
                         uint8_t value) {
  butterfly_index_t index = get_butterfly_index(move);
  (*butterfly_board)[index] = value;
}