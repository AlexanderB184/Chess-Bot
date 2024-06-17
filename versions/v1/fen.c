#include "fen.h"

int parse_row(String fen_string, size_t* index, size_t rank, Board* board) {
  size_t file = 0;
  while (file < 8) {
    char letter = fen_string.buffer[*index];
    Piece_Colour colour;
    Piece_Type type;

    if (letter >= '1' && letter <= '8') {
      file += letter - '0';  // empty
      if (fen_string.length <= ++(*index)) {
        return 1;
      }
      continue;
    }

    char upper_case_letter;

    if (letter >= 'A' && letter <= 'Z') {
      upper_case_letter = letter;
      colour = WHITE;
    } else if (letter >= 'a' && letter <= 'z') {
      upper_case_letter = letter + 'A' - 'a';
      colour = BLACK;
    } else {
      return 1;  // parse error
    }

    switch (upper_case_letter) {
      case 'K':
        type = KING;
        break;
      case 'Q':
        type = QUEEN;
        break;
      case 'R':
        type = ROOK;
        break;
      case 'B':
        type = BISHOP;
        break;
      case 'N':
        type = KNIGHT;
        break;
      case 'P':
        type = PAWN;
        break;
      default:
        return 1;  // parse error
    }
    board_set_piece(board, rank + file, (Piece){colour, type});

    file++;
    if (fen_string.length <= ++(*index)) {
      return 1;
    }
  }
  return file != 8;  // error if file is not exactly 8
}

#define FEN_PARSE_ERROR()                                                \
  fprintf(stderr, "ERROR: failed to parse FEN string. error occurred at byte offset %zu\n", index); \
  return (Board){0};

#define FEN_PARSE_ASSERT(cond) \
  if (!(cond)) {               \
    FEN_PARSE_ERROR();         \
  }

#define FEN_PARSE_NEXT(string, index) \
  FEN_PARSE_ASSERT((string).length > ++(index));

#define FEN_PARSE_EXPECT(string, index, token)           \
  FEN_PARSE_ASSERT((string).buffer[(index)] == (token)); \
  FEN_PARSE_NEXT((string), (index));

Board parse_FEN(String fen_string) {
  Board board = (Board){0};
  size_t index = 0;

  // pieces

  for (int rank = 56; rank >= 0; rank-=8) {
    int error = parse_row(fen_string, &index, rank, &board);
    FEN_PARSE_ASSERT(error == 0);
    if (rank != 0) {
      FEN_PARSE_EXPECT(fen_string, index, '/');
    }
  }
  FEN_PARSE_EXPECT(fen_string, index, ' ');  // skip space

  // next to move

  switch (fen_string.buffer[index]) {
    case 'w':
      board.turn = WHITE;
      break;
    case 'b':
      board.turn = BLACK;
      break;
    default:
      FEN_PARSE_ERROR();
  }
  FEN_PARSE_NEXT(fen_string, index);

  FEN_PARSE_EXPECT(fen_string, index, ' ');  // skip space

  // castle rights

  if (fen_string.buffer[index] == '-') {
    FEN_PARSE_NEXT(fen_string, index);
  } else {
    if (fen_string.buffer[index] == 'K') {
      board.rights |= WHITE_KING_SIDE;
      FEN_PARSE_NEXT(fen_string, index);
    }
    if (fen_string.buffer[index] == 'Q') {
      board.rights |= WHITE_QUEEN_SIDE;
      FEN_PARSE_NEXT(fen_string, index);
    }
    if (fen_string.buffer[index] == 'k') {
      board.rights |= BLACK_KING_SIDE;
      FEN_PARSE_NEXT(fen_string, index);
    }
    if (fen_string.buffer[index] == 'q') {
      board.rights |= BLACK_QUEEN_SIDE;
      FEN_PARSE_NEXT(fen_string, index);
    }
  }

  FEN_PARSE_EXPECT(fen_string, index, ' ');  // skip space

  // en passent

  if (fen_string.buffer[index] == '-') {
    board.enpassent = -1;
  
    FEN_PARSE_NEXT(fen_string, index);
  } else {
    FEN_PARSE_ASSERT(fen_string.buffer[index] >= 'a' &&
                     fen_string.buffer[index] <= 'h');
    board.enpassent = fen_string.buffer[index] - 'a';
    FEN_PARSE_NEXT(fen_string, index);
    FEN_PARSE_ASSERT(fen_string.buffer[index] == '3' ||
                     fen_string.buffer[index] == '6');
    board.enpassent += 8 * (fen_string.buffer[index] - '1');
    FEN_PARSE_NEXT(fen_string, index);
  }

  FEN_PARSE_EXPECT(fen_string, index, ' ');  // skip space

  // half move clock

  FEN_PARSE_ASSERT(fen_string.buffer[index] >= '0' &&
                   fen_string.buffer[index] <= '9');
  int half_move_clock = fen_string.buffer[index] - '0';
  FEN_PARSE_NEXT(fen_string, index);
  if (fen_string.buffer[index] != ' ') {
    FEN_PARSE_ASSERT(fen_string.buffer[index] >= '0' &&
                     fen_string.buffer[index] <= '9');
    half_move_clock = 10 * half_move_clock + fen_string.buffer[index] - '0';
    FEN_PARSE_NEXT(fen_string, index);
  }
  board.half_move_count = half_move_clock;

  FEN_PARSE_EXPECT(fen_string, index, ' ');  // skip space

  // full move counter
  FEN_PARSE_ASSERT(fen_string.buffer[index] >= '1' &&
                   fen_string.buffer[index] <= '9');
  int full_move_counter = fen_string.buffer[index] - '0';
  index++;
  while (index < fen_string.length) {
    FEN_PARSE_ASSERT(fen_string.buffer[index] >= '0' &&
                     fen_string.buffer[index] <= '9');
    full_move_counter = 10 * full_move_counter + fen_string.buffer[index] - '0';
    index++;
  }
  board.move_count = full_move_counter;
  return board;
}
