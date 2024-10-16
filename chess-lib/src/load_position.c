#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/chess-lib.h"
#include "../include/notation.h"
#include "../include/private/chess-lib-internals.h"

void release_position(chess_state_t* chess_state) {
  if (chess_state->ply_stack) {
    free(chess_state->ply_stack);
    chess_state->ply_stack = NULL;
  }
}

void clear_position(chess_state_t* chess_state) {
  release_position(chess_state);

  memset(chess_state, 0, sizeof(*chess_state));
  memcpy(chess_state->board, empty_board, sizeof(empty_board));

  chess_state->friendly_colour = WHITE;
  chess_state->enemy_colour = BLACK;
  chess_state->friendly_pieces = &chess_state->white_pieces;
  chess_state->enemy_pieces = &chess_state->black_pieces;
  chess_state->up_increment = 16, chess_state->down_increment = -16;
}

void init_ply_stack(chess_state_t* chess_state) {
  chess_state->ply_stack_capacity = chess_state->ply_counter + 50ull;
  chess_state->ply_stack = malloc(sizeof(ply_stack_item_t) * chess_state->ply_stack_capacity);
}

void load_start_position(chess_state_t* chess_state) {
  load_position(chess_state, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

void copy_position(chess_state_t* chess_state,
                   const chess_state_t* state_to_copy) {
  release_position(chess_state);
  memcpy(chess_state, state_to_copy, sizeof(chess_state_t));
  size_t ply_stack_size = sizeof(ply_stack_item_t) * chess_state->ply_stack_capacity;
  chess_state->ply_stack = malloc(ply_stack_size);
  memcpy(chess_state->ply_stack, state_to_copy->ply_stack, ply_stack_size);
  if (chess_state->black_to_move) {
    chess_state->friendly_pieces = &chess_state->black_pieces;
    chess_state->enemy_pieces = &chess_state->white_pieces;
  } else {
    chess_state->friendly_pieces = &chess_state->white_pieces;
    chess_state->enemy_pieces = &chess_state->black_pieces;
  }
}

piece_t parse_piece(char letter) {
  switch (letter) {
    case 'K': return WHITE_KING;
    case 'Q': return WHITE_QUEEN;
    case 'R': return WHITE_ROOK;
    case 'B': return WHITE_BISHOP;
    case 'N': return WHITE_KNIGHT;
    case 'P': return WHITE_PAWN;
    case 'k': return BLACK_KING;
    case 'q': return BLACK_QUEEN;
    case 'r': return BLACK_ROOK;
    case 'b': return BLACK_BISHOP;
    case 'n': return BLACK_KNIGHT;
    case 'p': return BLACK_PAWN;
    default:  return EMPTY;
  }
}



#ifdef PRINT_READ_ERRORS
#define READ_ERROR(msg, args...) {printf("READ ERROR: reading \"%s\" ", buffer);printf(msg, ##args); return -1;}
#else
#define READ_ERROR(msg, args...) {return -1;}
#endif

#ifdef PRINT_WRITE_ERRORS
#define WRITE_ERROR(msg, args...) {printf("WRITE ERROR: ");printf(msg, ##args); buffer[0] = 0; return -1;}
#else
#define WRITE_ERROR(msg, args...) {buffer[0] = 0; return -1;}
#endif

long load_position(chess_state_t* chess_state, const char* buffer) {
  long bytes_read = 0;
  long size = strlen(buffer) + 1;

  clear_position(chess_state);

  // pieces
  if (buffer[bytes_read] == '\0' || bytes_read >= size) {
    READ_ERROR("fen missing piece table.\n");
  }
  for (int rank = 7; rank >= 0; rank--) {
    int file = 0;
    while (file < 8) {
      if (bytes_read >= size) READ_ERROR("fen missing piece table\n");
      char letter = buffer[bytes_read++];
      if (letter >= '1' && letter <= '8') {
        file += letter - '0';
        continue;
      }
      piece_t piece_to_add = parse_piece(letter);
      if (piece_to_add == EMPTY) {
        READ_ERROR("invalid piece \'%c\' in fen piece table\n", letter);
      }
      place_piece(chess_state, rankfile_to_sq0x88(rank, file), piece_to_add);
      file++;
    }
    if (file != 8) {
      READ_ERROR("too many pieces in rank %d\n", rank);
    }
    if (rank != 0 && (bytes_read >= size || buffer[bytes_read++] != '/')) {
      READ_ERROR("row in fen piece table not seperated with \'/\'");
    }
  }
  
  bytes_read += skip_whitespace(buffer + bytes_read);

  // next to move
  if (buffer[bytes_read] == '\0' || bytes_read >= size) {
    READ_ERROR("fen missing player to move.\n");
  }
  char who2move = buffer[bytes_read++];
  switch (who2move) {
    case 'w':
      break;
    case 'b':
      update_turn(chess_state);
      break;
    default:
      READ_ERROR("invalid code for player to move \'%c\', must be \'w\' or \'b\'.\n", who2move);
  }

  bytes_read += skip_whitespace(buffer + bytes_read);
  // castle rights
  if (buffer[bytes_read] == '\0' || bytes_read >= size) {
    READ_ERROR("fen missing castle rights.\n");
  }

  if (bytes_read < size && buffer[bytes_read] == '-') {
    bytes_read++;
  } else {
    if (bytes_read < size && buffer[bytes_read] == 'K') {
      chess_state->castle_rights |= WHITE_KING_SIDE;
      bytes_read++;
    }
    if (bytes_read < size && buffer[bytes_read] == 'Q') {
      chess_state->castle_rights |= WHITE_QUEEN_SIDE;
      bytes_read++;
    }
    if (bytes_read < size && buffer[bytes_read] == 'k') {
      chess_state->castle_rights |= BLACK_KING_SIDE;
      bytes_read++;
    }
    if (bytes_read < size && buffer[bytes_read] == 'q') {
      chess_state->castle_rights |= BLACK_QUEEN_SIDE;
      bytes_read++;;
    }
  }

  bytes_read += skip_whitespace(buffer + bytes_read);

  // en passent
  if (buffer[bytes_read] == '\0' || bytes_read >= size) {
    READ_ERROR("fen missing enpassent target.\n");
  }

  if (bytes_read < size && buffer[bytes_read] == '-') {
    chess_state->enpassent_target = -1;
    bytes_read++;
  } else {

    int out = read_square(buffer + bytes_read, size - bytes_read, &chess_state->enpassent_target);
    if (out == -1) return -1;
    bytes_read += out;
    if (sq0x88_to_rank07(chess_state->enpassent_target) != 3 && sq0x88_to_rank07(chess_state->enpassent_target) != 4) {
      READ_ERROR("fen contains invalid enpassent target.\n");
    }
  }

  bytes_read += skip_whitespace(buffer + bytes_read);
  // everything past here is optional

  // half move clock
  if (bytes_read < size && buffer[bytes_read] >= '0' && buffer[bytes_read] <= '9') {
    int half_move_clock = buffer[bytes_read++] - '0';
    if (bytes_read < size && buffer[bytes_read] >= '0' && buffer[bytes_read] <= '9') {
      half_move_clock = 10 * half_move_clock + buffer[bytes_read] - '0';
    }
    chess_state->half_move_clock = half_move_clock;
    bytes_read += skip_whitespace(buffer + bytes_read);
  }


  // full move counter
  if (bytes_read < size && buffer[bytes_read] >= '1' && buffer[bytes_read] <= '9') {
    int move_counter = buffer[bytes_read++] - '0';
    if (bytes_read < size && buffer[bytes_read] >= '0' && buffer[bytes_read] <= '9') {
      move_counter = 10 * move_counter + buffer[bytes_read] - '0';
    }
    chess_state->ply_counter = (move_counter-1) * 2 + chess_state->black_to_move;
  }

  chess_state->ply_of_last_irreversible_move = chess_state->ply_counter;

  init_check(chess_state);
  init_ply_stack(chess_state);
  return bytes_read;
}