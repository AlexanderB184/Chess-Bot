#include "../include/move-order.h"

#include "../include/butterfly-board.h"
#include "../include/static-evaluation.h"
#include "../include/static-exchange-evaluation.h"

// priority
// 1. transpo-table move
// 2. winning captures (see > 0) and promotions (ordered using value difference
// (value(to) - value(from) + value(promote_to)))
// 3. neutral captures (ordered using value difference (value(to) -
// value(from)))
// 4. killer moves
// 5. losing captures (ordered using value difference (value(to) - value(from)))
// 6. quiet moves (ordered using history heuristic)

// priority without sub ordering, TODO add subordering to order within the
// categories

uint8_t history_heuristic[64 * 64];
uint8_t butterfly_heuristic[64 * 64];

uint8_t piece_value_heuristic(piece_t piece) {
  switch (piece & PIECE_MASK) {
    case KING:
      return 200;
      break;
    case QUEEN:
      return 90;
      break;
    case ROOK:
      return 50;
      break;
    case BISHOP:
      return 33;
      break;
    case KNIGHT:
      return 32;
      break;
    case PAWN:
      return 10;
      break;
    default:
      return 0;
  }
}

int piece_value_difference_heuristic(const chess_state_t* chess_state,
                                     move_t move) {
  piece_t capturing_piece = piece(chess_state, get_from(move));
  piece_t captured_piece = piece(chess_state, get_to(move));
  return piece_value_heuristic(captured_piece) -
         piece_value_heuristic(capturing_piece) + 200;
  // value between 10 (king takes pawn) and 280 (pawn takes queen)
  // pp: 200
  // pb: 223
  // pn: 222
  // pr: 240
  // pq: 280
  // np: 178
  // nn: 200
  // nb: 201
  // nr: 218
  // nq: 258
  // bp: 177
  // bn: 199
  // bb: 200
  // br: 217
  // bq: 257
  // rp: 150
  // rb: 173
  // rn: 172
  // rr: 190
  // rq: 230
  // qp: 120
  // qn: 142
  // qb: 143
  // qr: 160
  // qq: 200
  // kp: 10
  // kn: 32
  // kb: 33
  // kr: 50
  // kq: 90
}

int calculate_priority(const chess_state_t* chess_state, move_t move,
                       move_t previous_best_move) {
  if (compare_moves(previous_best_move, move)) {
    return PRIORITY_MAX;
  }
  if (is_promotion(move)) {
    return PRIORITY_WINNING_CAPTURE_SEQUENCE +
           piece_value_heuristic(get_promotes_to(move)) +
           piece_value_difference_heuristic(
               chess_state, move);  // 190 if it isn't a capture, 222 for knight
                                    // 223 for bishop 240 for rook 280 for queen
  }
  if (is_capture(move)) {
    score_centipawn_t see = static_exchange_evaluation(chess_state, move);
    if (see > 0) {
      return PRIORITY_WINNING_CAPTURE_SEQUENCE +
             piece_value_difference_heuristic(chess_state, move);
    }
    if (see == 0) {
      return PRIORITY_NEUTRAL_CAPTURE_SEQUENCE +
             piece_value_difference_heuristic(chess_state, move);
    }
    if (see < 0) {
      return PRIORITY_LOSING_CAPTURE_SEQUENCE +
             piece_value_difference_heuristic(chess_state, move);
    }
  }
  // if (is_killer_move(move)) {
  //   return KILLER_MOVE;
  // }
  int relative_butterfly_heuristic =
      get_butterfly_board(butterfly_heuristic, move) == 0
          ? 0
          : (0x100 * get_butterfly_board(history_heuristic, move)) /
                get_butterfly_board(butterfly_heuristic, move);
  return PRIORITY_QUIET_MOVE + relative_butterfly_heuristic;
}

move_t set_priority(move_t move, int priority) {
  move.bitpacked_data = (move.bitpacked_data & 0xF) | (priority << 4);
  return move;
}

void generate_priorities(const chess_state_t* chess_state, move_t* moves,
                         size_t move_count, move_t hash_move) {
  for (size_t i = 0; i < move_count; i++) {
    moves[i] = set_priority(
        moves[i], calculate_priority(chess_state, moves[i], hash_move));
  }
}

move_t next_move(const chess_state_t* chess_state, move_t* moves,
                 size_t* move_count) {
  score_centipawn_t best_move_priority = MIN_SCORE;
  size_t best_move_index = *move_count;
  for (size_t i = 0; i < *move_count; i++) {
    if (get_priority(moves[i]) > best_move_priority) {
      if (is_legal(chess_state, moves[i])) {
        best_move_priority = get_priority(moves[i]);
        best_move_index = i;
      } else {
        moves[i--] = moves[--*move_count];  // swap erase illegal moves
      }
    }
  }

  if (*move_count == 0) {
    return null_move;
  }

  // swap erase best move once found so it cant be taken again
  move_t ret = moves[best_move_index];
  moves[best_move_index] = moves[--*move_count];
  return ret;
}

move_t remove_move(const chess_state_t* chess_state, move_t* moves,
                   size_t* move_count, move_t hash_move) {
  if (!is_legal(chess_state, hash_move)) {
    return null_move;
  }
  for (size_t i = 0; i < *move_count; i++) {
    if (compare_moves(moves[i], hash_move)) {
      moves[i] = moves[--*move_count];
      return hash_move;
    }
  }
  return null_move;
}

move_t select_best_move(move_t* moves, score_centipawn_t* scores,
                        size_t move_count, size_t index) {
  size_t best_index = index;
  score_centipawn_t best_score = scores[index];
  for (size_t i = index + 1; i < move_count; i++) {
    if (scores[i] > best_score) {
      best_score = scores[i];
      best_index = i;
    }
  }
  if (best_index == index) {
    return moves[index];
  }
  score_centipawn_t first_score = scores[index];
  move_t first_move = moves[index];
  scores[index] = scores[best_index];
  moves[index] = moves[best_index];
  scores[best_index] = first_score;
  moves[best_index] = first_move;
  return moves[index];
}