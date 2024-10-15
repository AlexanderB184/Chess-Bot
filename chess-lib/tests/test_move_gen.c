#include <stdio.h>

#include "../include/chess-lib-constants.h"
#include "../include/chess-lib.h"

static chess_state_t chess_state;

void test(const char* fen_string, size_t expected_move_count) {
  static int testno = 0;
  testno++;
  load_position(&chess_state, fen_string);
  move_t moves[256];
  size_t move_count = generate_legal_moves(&chess_state, moves);
  if (move_count == expected_move_count) {
    printf("passed %d\n", testno);
  } else {
    printf("failed %d: expected: %zu got: %zu\n", testno, expected_move_count,
           move_count);
    for (int i = 0; i < move_count; i++) {
      printf("from: %d to: %d flags: %d\n", get_from(moves[i]),
             get_to(moves[i]), get_flags(moves[i]));
    }
  }
}

int main(int argc, char const* argv[]) {
  test("8/8/8/8/8/8/8/R3K2k w Q - 0 1", 16);
  test("8/8/8/8/8/8/8/R3K2k b Q - 0 1", 3);
  test("4r3/8/8/8/8/8/8/R3K2k w Q - 0 1", 4);
  test("8/8/8/8/8/8/8/R3K1k1 b Q - 0 1", 3);
  test("8/8/8/3pP3/4K3/8/8/6k1 w - d6 0 1", 8);
  test("4r3/8/8/3pP3/4K3/8/8/6k1 w - d6 0 1", 7);
  return 0;
}
