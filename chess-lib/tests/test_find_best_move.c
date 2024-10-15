#include <stdio.h>

#include "../chess-bot/include/debug.h"
#include "../chess-bot/include/iterative-deepening.h"

debug_info_t debug_info;

int main(int argc, char const *argv[]) {
  chess_state_t chess_state = {};
  load_position(&chess_state,"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
  (void)find_best_move(&chess_state);
  debug_info_t debug = debug_info;
  printf("time elapsed: %.2fms\n", (double)debug.time_elapsed_ms / 1000000);
  printf("depth reached: %zu\n", debug.max_depth_reached);
  printf("last iteration completeness: %.2f\n", (double)debug.moves_evaluated_in_final_iteration/debug.possible_moves);

  printf("total nodes searched: %zu\n", debug.total_nodes_searched);
  printf("unique nodes searched: %zu\n", debug.unique_nodes_searched);
  printf("quiescence nodes: %zu\n", debug.quiescence_nodes_searched);
  printf("check extension nodes: %zu\n", debug.check_extension_nodes_searched);

  printf("beta cutoff rate: %f\n",
         (double)debug.beta_cutoffs / debug.total_nodes_searched);
  printf(
      "transpostion table cutoff rate: %f\n",
      (double)debug.transposition_table_cutoffs / debug.total_nodes_searched);
        printf(
      "delta cutoff rate: %f\n",
      (double)debug.delta_cutoffs / debug.quiescence_nodes_searched);

  return 0;
}
