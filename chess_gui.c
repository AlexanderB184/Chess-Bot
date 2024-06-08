#include "chess_gui.h"

#include "move_generation.h"
#include "move_validation.h"

Move get_legal_move(FILE* in, const Board* board) {
  Scratch_Arena scratch = get_scratch(NULL, 0);
  if (generate_legal_moves(scratch.arena, board).length == 0) {
    release_scratch(scratch);
    return (Move){0};  // no legal moves
  }
  Move move;
  do {
    move = get_move(in, board);
  } while (!is_legal_move(board, move));
  release_scratch(scratch);
  return move;
}

Move get_move(FILE* in, const Board* board) {
  char cmd[64];
  (void)fgets(cmd, sizeof(cmd), in);
  String move = strip_string(from_cstr(cmd));
  return parse_move(board, move);
}

Move parse_move(const Board* board, String command) {
  // start (file)(row)
  // end (file)(row)
  // promote (promote to) (optional)

  if (command.length < 4 || command.length > 5) {
    return (Move){0};
  }

  Move move;
  if (!(command.buffer[0] >= 'a' && command.buffer[0] <= 'h' &&
        command.buffer[1] >= '1' && command.buffer[1] <= '8' &&
        command.buffer[2] >= 'a' && command.buffer[2] <= 'h' &&
        command.buffer[3] >= '1' && command.buffer[3] <= '8')) {
    return (Move){0};
  }

  move.piece_start_col = command.buffer[0] - 'a';
  move.piece_start_row = command.buffer[1] - '1';
  move.piece_end_col = command.buffer[2] - 'a';
  move.piece_end_row = command.buffer[3] - '1';

  if (command.length == 4) {
    return move;
  }

  switch (command.buffer[4]) {
    case 'Q':
      move.promote_to = QUEEN;
      break;
    case 'R':
      move.promote_to = ROOK;
      break;
    case 'B':
      move.promote_to = BISHOP;
      break;
    case 'N':
      move.promote_to = KNIGHT;
      break;
    default:
      return (Move){0};
  }

  // check for castle

  // check for enpassent

  return move;
}

void display_board(FILE* out, const Board* board, Piece_Colour turn) {
  fprintf(out, "\033[u\033[J");
  size_t r = turn == WHITE ? 7 : 0;
  size_t dr = turn == WHITE ? -1 : 1;
  // fprintf(out, " abcdefgh\n");
  for (; r < 8 && r >= 0; r += dr) {
    putc(r + '1', out);
    for (size_t c = 0; c < 8; c++) {
      Piece p = board->pieces[r][c];
      if (p.colour == NONE) {
        putc(' ', out);
        continue;
      }
      char char_to_print;
      switch (p.type) {
        case EMPTY:
          char_to_print = ' ';
          break;
        case KING:
          char_to_print = 'k';
          break;
        case QUEEN:
          char_to_print = 'q';
          break;
        case ROOK:
          char_to_print = 'r';
          break;
        case BISHOP:
          char_to_print = 'b';
          break;
        case KNIGHT:
          char_to_print = 'n';
          break;
        case PAWN:
          char_to_print = 'p';
          break;
      }
      if (p.colour == BLACK) {
        char_to_print += 'A' - 'a';
      }
      putc(char_to_print, out);
    }
    putc('\n', out);
  }
  fflush(out);  // Ensure the output is immediately written to the terminal
}
