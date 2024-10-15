#include <stdio.h>

#include "chess-bot/include/transpo-table.h"

size_t search(chess_state_t* chess_state, table_t* table, size_t depth,
              size_t max_depth) {
  if (depth == max_depth) {
    return 1;
  }

  size_t count = 0;

  node_index_t index = search_table(table, chess_state->zobrist);
  if (index != NULL_INDEX) {
    node_t node = get_node(table, index);
    if (node_get_age(node) == max_depth) {
      if (node_get_depth(node) == depth) {
        return (size_t)node_get_score(node);
      }
    }
  }
  move_t moves[256];
  size_t move_count = generate_legal_moves(chess_state, moves);

  for (int i = 0; i < move_count; i++) {
    make_move(chess_state, moves[i]);
    count += search(chess_state, table, depth + 1, max_depth);
    unmake_move(chess_state);
  }

  update_table(table, index, chess_state->zobrist,
               make_node(null_move, count, 0, max_depth, depth));

  return count;
}

int main(int argc, char const* argv[]) {
  chess_state_t chess_state = {};
  table_t table = {};
  make_table(&table, 1000, 300);
  load_start_position(&chess_state);
  for (int i = 0; i <= 7; i++) {
    size_t count = search(&chess_state, &table, 0, i);
    printf("%zu\n", count);
  }
  return 0;
}
