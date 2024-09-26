#include <string.h>

#include "chess-bot/include/iterative-deepening.h"
#include "chess-bot/include/transposition-table.h"
int on_start();
int on_stop();
int stop();

static chess_state_t match_state;
//static table_t table;

int main(int argc, char const *argv[]) {
  //make_table(&table, 1000, 300);
  load_position(&match_state,
                "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
  on_start();

  move_t best_move;

  move_t moves[256];
  size_t move_count = generate_legal_moves(&match_state, moves);
  score_centipawn_t scores[256] = {};

  for (int max_depth = 0; max_depth < 12; max_depth++) {
    best_move = find_best_move_iteration(&match_state, moves, scores,
                                         move_count, max_depth);
  }
  on_stop();
  (void)best_move;

  return 0;
}
