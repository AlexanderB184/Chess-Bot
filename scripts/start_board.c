#include <stdio.h>

#include "chess-lib-types.h"

void print_enum(piece_t piece) {
  switch (piece) {
    case EMPTY:
      printf("EMPTY       ");
      break;
    case BORDER:
      printf("BORDER      ");
      break;
    case WHITE_PAWN:
      printf("WHITE_PAWN  ");
      break;
    case WHITE_KNIGHT:
      printf("WHITE_KNIGHT");
      break;
    case WHITE_BISHOP:
      printf("WHITE_BISHOP");
      break;
    case WHITE_ROOK:
      printf("WHITE_ROOK  ");
      break;
    case WHITE_QUEEN:
      printf("WHITE_QUEEN ");
      break;
    case WHITE_KING:
      printf("WHITE_KING  ");
      break;
    case BLACK_PAWN:
      printf("BLACK_PAWN  ");
      break;
    case BLACK_KNIGHT:
      printf("BLACK_KNIGHT");
      break;
    case BLACK_BISHOP:
      printf("BLACK_BISHOP");
      break;
    case BLACK_ROOK:
      printf("BLACK_ROOK  ");
      break;
    case BLACK_QUEEN:
      printf("BLACK_QUEEN ");
      break;
    case BLACK_KING:
      printf("BLACK_KING  ");
      break;
    default:
      break;
  }
}

int main(int argc, char const *argv[]) {
  piece_t board[16 * 16];
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      if (i >= 8 || j >= 8) {
        board[i * 16 + j] = BORDER;
      }
      if (j < 8 && i > 1 && i < 6) {
        board[i * 16 + j] = EMPTY;
      }
      if (j < 8 && i == 1) {
        board[i * 16 + j] = WHITE_PAWN;
      }
      if (j < 8 && i == 6) {
        board[i * 16 + j] = BLACK_PAWN;
      }
    }
  }
  board[0] = WHITE_ROOK;
  board[1] = WHITE_KNIGHT;
  board[2] = WHITE_BISHOP;
  board[3] = WHITE_QUEEN;
  board[4] = WHITE_KING;
  board[5] = WHITE_BISHOP;
  board[6] = WHITE_KNIGHT;
  board[7] = WHITE_ROOK;
  board[0 + 7 * 16] = BLACK_ROOK;
  board[1 + 7 * 16] = BLACK_KNIGHT;
  board[2 + 7 * 16] = BLACK_BISHOP;
  board[3 + 7 * 16] = BLACK_QUEEN;
  board[4 + 7 * 16] = BLACK_KING;
  board[5 + 7 * 16] = BLACK_BISHOP;
  board[6 + 7 * 16] = BLACK_KNIGHT;
  board[7 + 7 * 16] = BLACK_ROOK;

  printf("static const piece_t start_board[16 * 16] = {\n");

  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      print_enum(board[i * 16 + j]);
      printf(", ");
    }
    printf("\n");
  }
  printf("};\n");

  return 0;
}
