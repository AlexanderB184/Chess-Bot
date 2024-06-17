#include "move_generation.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "move_validation.h"

Slice_impl(Move);

void generate_psuedo_legal_pawn_moves(Slice(Move) buffer, Slice(Move) * moves,
                                      const Board* board, int from) {
  int dir = board->turn == WHITE ? 1 : -1;
  int row = rank_of(from), col = file_of(from);
  // single forward step
  if (row + dir >= 0 && row + dir < 8 &&
      board->pieces[index_of(row + dir, col)].type == EMPTY) {
    if (row + dir == 0 || row + dir == 7) {
      Move move = (Move){from, index_of(row + dir, col), QUEEN};
      if (buffer.length < moves->length + 4) {
        fprintf(stderr,
                "MOVE GEN ERROR: buffer too small, truncating move list\n");
        return;
      }
      moves->buffer[moves->length++] = move;
      move.promote_to = ROOK;
      moves->buffer[moves->length++] = move;
      move.promote_to = BISHOP;
      moves->buffer[moves->length++] = move;
      move.promote_to = KNIGHT;
      moves->buffer[moves->length++] = move;

    } else {
      Move move = (Move){from, index_of(row + dir, col), EMPTY};
      if (buffer.length <= moves->length) {
        fprintf(stderr,
                "MOVE GEN ERROR: buffer too small, truncating move list\n");
        return;
      }
      moves->buffer[moves->length++] = move;
    }
  }

  // double forward step
  // Double forward step
  if ((row == 1 && board->turn == WHITE) ||
      (row == 6 && board->turn == BLACK)) {
    if (row + 2 * dir >= 0 && row + 2 * dir < 8 &&
        board->pieces[index_of(row + dir, col)].type == EMPTY &&
        board->pieces[index_of(row + 2 * dir, col)].type == EMPTY) {
      Move move = (Move){from, index_of(row + 2 * dir, col), EMPTY};
      if (buffer.length <= moves->length) {
        fprintf(stderr,
                "MOVE GEN ERROR: buffer too small, truncating move list\n");
        return;
      }
      moves->buffer[moves->length++] = move;
    }
  }

  // capture and enpassent right
  if (col + 1 < 8 && row + dir >= 0 && row + dir < 8 &&
      (board->pieces[index_of(row + dir, col + 1)].colour == -board->turn ||
       (board->enpassent == index_of(row, col + 1)))) {
    if (row + dir == 0 || row + dir == 7) {
      Move move = (Move){from, index_of(row + dir, col + 1), QUEEN};
      if (buffer.length < moves->length + 4) {
        fprintf(stderr,
                "MOVE GEN ERROR: buffer too small, truncating move list\n");
        return;
      }
      moves->buffer[moves->length++] = move;
      move.promote_to = ROOK;
      moves->buffer[moves->length++] = move;
      move.promote_to = BISHOP;
      moves->buffer[moves->length++] = move;
      move.promote_to = KNIGHT;
      moves->buffer[moves->length++] = move;
    } else {
      Move move = (Move){from, index_of(row + dir, col + 1), EMPTY};
      if (buffer.length <= moves->length) {
        fprintf(stderr,
                "MOVE GEN ERROR: buffer too small, truncating move list\n");
        return;
      }
      moves->buffer[moves->length++] = move;
    }
  }

  // capture and enpassent left
  if (col - 1 >= 0 && row + dir >= 0 && row + dir < 8 &&
      (board->pieces[index_of(row + dir, col - 1)].colour ==
           -board->turn ||
       (board->enpassent == index_of(row, col - 1)))) {
    if (row + dir == 0 || row + dir == 7) {
      Move move = (Move){from, index_of(row + dir, col - 1), QUEEN};
      if (buffer.length < moves->length + 4) {
        fprintf(stderr,
                "MOVE GEN ERROR: buffer too small, truncating move list\n");
        return;
      }
      moves->buffer[moves->length++] = move;
      move.promote_to = ROOK;
      moves->buffer[moves->length++] = move;
      move.promote_to = BISHOP;
      moves->buffer[moves->length++] = move;
      move.promote_to = KNIGHT;
      moves->buffer[moves->length++] = move;
    } else {
      Move move = (Move){from, index_of(row + dir, col - 1), EMPTY};
      if (buffer.length <= moves->length) {
        fprintf(stderr,
                "MOVE GEN ERROR: buffer too small, truncating move list\n");
        return;
      }
      moves->buffer[moves->length++] = move;
    }
  }
}
#include <stdio.h>
void generate_psuedo_legal_knight_moves(Slice(Move) buffer, Slice(Move) * moves,
                                        const Board* board, int from) {
  static const int offsets[8][2] = {{1, 2}, {-1, 2}, {1, -2}, {-1, -2},
                                    {2, 1}, {2, -1}, {-2, 1}, {-2, -1}};
  int row = rank_of(from), col = file_of(from);
  for (size_t k = 0; k < 8; k++) {
    int target_row = row + offsets[k][0], target_col = col + offsets[k][1];
    if (target_row >= 0 && target_row < 8 && target_col >= 0 &&
        target_col < 8 &&
        board->pieces[index_of(target_row, target_col)].colour != board->turn) {
      Move move = (Move){from, index_of(target_row, target_col), EMPTY};
      if (buffer.length <= moves->length) {
        fprintf(stderr,
                "MOVE GEN ERROR: buffer too small, truncating move list\n");
        return;
      }
      moves->buffer[moves->length++] = move;
    }
  }
}
void generate_psuedo_legal_sliding_moves(Slice(Move) buffer,
                                         Slice(Move) * moves,
                                         const Board* board, int from) {
  int row = rank_of(from), col = file_of(from);
  // Sliding moves
  for (int d_row = -1; d_row <= 1; d_row++) {
    for (int d_col = -1; d_col <= 1; d_col++) {
      if (d_col == 0 && d_row == 0) {
        continue;
      }
      if ((((d_row == 0 || d_col == 0) && board->pieces[from].type == BISHOP) ||
           ((d_row != 0 && d_col != 0) &&
            (board->pieces[from].type == ROOK)))) {
        continue;
      }
      int target_row = row + d_row;
      int target_col = col + d_col;
      while (target_row >= 0 && target_row < 8 && target_col >= 0 &&
             target_col < 8) {
        if (board->pieces[index_of(target_row, target_col)].colour ==
            board->turn) {
          break;
        }
        Move move = (Move){from, index_of(target_row, target_col), EMPTY};
        if (buffer.length <= moves->length) {
          fprintf(stderr,
                  "MOVE GEN ERROR: buffer too small, truncating move list\n");
          return;
        }
        moves->buffer[moves->length++] = move;

        if (board->pieces[index_of(target_row, target_col)].type != EMPTY) {
          break;
        }
        target_row += d_row;
        target_col += d_col;
      }
    }
  }
}

void generate_psuedo_legal_king_moves(Slice(Move) buffer, Slice(Move) * moves,
                                      const Board* board, int from) {
  int row = rank_of(from), col = file_of(from);
  for (int d_row = -1; d_row <= 1; d_row++) {
    for (int d_col = -1; d_col <= 1; d_col++) {
      if (d_row == 0 && d_col == 0) {
        continue;
      }
      int target_row = row + d_row;
      int target_col = col + d_col;
      if (target_row >= 0 && target_row < 8 && target_col >= 0 &&
          target_col < 8 &&
          board->pieces[index_of(target_row, target_col)].colour !=
              board->turn) {
        Move move = (Move){from, index_of(target_row, target_col), EMPTY};
        if (buffer.length <= moves->length) {
          fprintf(stderr,
                  "MOVE GEN ERROR: buffer too small, truncating move list\n");
          return;
        }
        moves->buffer[moves->length++] = move;
      }
    }
  }

  // if king in check then cannot castle
  if (can_be_attacked_by(board, -board->turn, from)) {
    return;
  }

  // queen side castle
  if (((board->turn == WHITE && (board->rights & WHITE_QUEEN_SIDE)) ||
       (board->turn == BLACK && (board->rights & BLACK_QUEEN_SIDE))) &&
      board->pieces[index_of(row, 1)].type == EMPTY &&
      board->pieces[index_of(row, 2)].type == EMPTY &&
      board->pieces[index_of(row, 3)].type == EMPTY &&
      !can_be_attacked_by(board, -board->turn, index_of(row, 2)) &&
      !can_be_attacked_by(board, -board->turn, index_of(row, 3))) {
    Move move = (Move){from, index_of(row, 2), EMPTY};
    if (buffer.length <= moves->length) {
      fprintf(stderr,
              "MOVE GEN ERROR: buffer too small, truncating move list\n");
      return;
    }
    moves->buffer[moves->length++] = move;
  }

  // king side castle
  if (((board->turn == WHITE && (board->rights & WHITE_KING_SIDE)) ||
       (board->turn == BLACK && (board->rights & BLACK_KING_SIDE))) &&
      board->pieces[index_of(row, 5)].type == EMPTY &&
      board->pieces[index_of(row, 6)].type == EMPTY &&
      !can_be_attacked_by(board, -board->turn, index_of(row, 5)) &&
      !can_be_attacked_by(board, -board->turn, index_of(row, 6))

  ) {
    Move move = (Move){from, index_of(row, 6), EMPTY};
    if (buffer.length <= moves->length) {
      fprintf(stderr,
              "MOVE GEN ERROR: buffer too small, truncating move list\n");
      return;
    }
    moves->buffer[moves->length++] = move;
  }
}

// TODO: improve this so it isn't O(n^4)
Slice(Move)
    generate_psuedo_legal_moves(Slice(Move) buffer, const Board* board) {
  Slice(Move) moves = (Slice(Move)){.buffer = buffer.buffer, .length = 0};
  for (size_t from = 0; from < 64; from++) {
    if (board->turn != board->pieces[from].colour) {
      continue;
    }
    switch (board->pieces[from].type) {
      case EMPTY:
        break;
      case PAWN:
        generate_psuedo_legal_pawn_moves(buffer, &moves, board, from);
        break;
      case KNIGHT:
        generate_psuedo_legal_knight_moves(buffer, &moves, board, from);
        break;
      case BISHOP:
      case ROOK:
      case QUEEN:
        generate_psuedo_legal_sliding_moves(buffer, &moves, board, from);
        break;
      case KING:
        generate_psuedo_legal_king_moves(buffer, &moves, board, from);
        break;
      default:
        assert(0 && "Unknown piece type encountered");
        break;
    }
  }
  return moves;
}

Slice(Move) generate_legal_moves(Slice(Move) buffer, const Board* board) {
  Slice(Move) moves = generate_psuedo_legal_moves(buffer, board);

  for (size_t i = 0; i < moves.length; i++) {
    if (is_check_after_move(board, moves.buffer[i])) {
      moves.buffer[i] = moves.buffer[--moves.length];
      i--;
    }
  }
  return moves;
}