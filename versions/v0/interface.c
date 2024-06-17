#include "interface.h"

INTERFACE_EXIT_CODES run_bot(FILE* in, FILE* out,
                             find_best_move_fn* find_best_move) {
  Board board;
  Piece_Colour playing_as;
  INTERFACE_EXIT_CODES init_success = await_init(in, out, &board, &playing_as);
  if (init_success != OKAY) {
    return init_success;
  }
  loop {
    if (board.turn == playing_as) {
      Move move = find_best_move(&board);
      make_move(&board, move);
      send_move(in, out, move);
      INTERFACE_EXIT_CODES response = await_response(in);
      if (response != OKAY) {
        return response;
      }
    }
    if (is_gameover(&board)) {
      INTERFACE_EXIT_CODES confirmation = await_confirmation(in);
      if (confirmation == OKAY) {
        return GAMEOVER;
      }
      return confirmation;
    }
    Move move = await_move(in, &board);
    if (!is_legal_move(&board, move)) {
      fprintf(out, "illegal\n");
      return ILLEGAL_MOVE;
    }
    fprintf(out, "okay\n");

    make_move(&board, move);

    if (is_gameover(&board)) {
      fprintf(out, "gameover\n");
      return GAMEOVER;
    }
  }
}

INTERFACE_EXIT_CODES await_init(FILE* in, FILE* out, Board* board,
                                Piece_Colour* playing_as) {
  char init_msg[256];
  char* fgets_result = fgets(init_msg, sizeof(init_msg), in);
  if (fgets_result == NULL) {
    fprintf(stderr, "ERROR: fgets failed\n");
    return IO_FAILURE;
  }
  String message = strip_string(from_cstr(init_msg));
  String buffer[16];
  size_t index = find_char(message, '\n');
  String fen_string = substring(message, 0, index);
  *board = parse_FEN(fen_string);
  message = substring(message, index + 1, message.length);
  if (message.length <= 0) {
    return INVALID_MESSAGE;
  }
  if (string_compare(message, string_literal("white"))) {
    *playing_as = WHITE;
  } else if (string_compare(message, string_literal("black"))) {
    *playing_as = BLACK;
  } else {
    return INVALID_MESSAGE;
  }
  return OKAY;
}

INTERFACE_EXIT_CODES await_confirmation(FILE* in) {
  char res[64];
  char* fgets_result = fgets(res, sizeof(res), in);
  if (fgets_result == NULL) {
    fprintf(stderr, "ERROR: fgets failed\n");
    return IO_FAILURE;
  }
  String response = strip_string(from_cstr(res));
  if (string_compare(response, string_literal("gameover"))) {
    return OKAY;
  }
  return INVALID_MESSAGE;
}

int send_move(FILE* in, FILE* out, Move move) {
  char msg[16];
  msg[0] = 'a' + rank(move.from);
  msg[1] = '1' + rank(move.to);
  msg[2] = 'a' + rank(move.from);
  msg[3] = '1' + rank(move.to);
  msg[4] = move.promote_to == QUEEN    ? 'q'
           : move.promote_to == ROOK   ? 'r'
           : move.promote_to == BISHOP ? 'b'
           : move.promote_to == KNIGHT ? 'n'
                                       : 0;
  msg[5] = 0;
  return fprintf(out, "%s\n", msg);
}

INTERFACE_EXIT_CODES await_response(FILE* in) {
  // some sort of thing to check for timeout, add a time limit, e.g. 10 seconds
  char res[64];
  char* fgets_result = fgets(res, sizeof(res), in);
  if (fgets_result == NULL) {
    fprintf(stderr, "ERROR: fgets failed\n");
    return IO_FAILURE;
  }
  String response = strip_string(from_cstr(res));
  if (string_compare(response, string_literal("okay"))) {
    return OKAY;
  }
  if (string_compare(response, string_literal("illegal"))) {
    return ILLEGAL_MOVE;
  }
  return INVALID_MESSAGE;
}

void send_response(FILE* out, const Board* board, Move move) {
  if (!is_legal_move(board, move)) {
    fprintf(out, "illegal\n");
  }
  fprintf(out, "okay\n");
}

Move await_move(FILE* in, const Board* board) {
  char cmd[64];
  char* c = fgets(cmd, sizeof(cmd), in);
  if (!c) {
    fprintf(stderr, "ERROR: fgets failed\n");
    return (Move){0};
  }
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

  move.from = command.buffer[0] - 'a' + 8 * (command.buffer[1] - '1');
  move.to = command.buffer[2] - 'a' + 8 * (command.buffer[3] - '1');

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