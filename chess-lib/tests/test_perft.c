#include <stdio.h>

#include "../include/chess-lib.h"
#include "../include/perft.h"

static chess_state_t chess_state;

typedef struct {
  const char* const position;
  const perft_results_t* const results_table;
  int depth_of_first_row;
  int depth_of_largest_row;
  perft_active_columns flags;
} perft_test_t;

void test_perft(perft_test_t test_case) {
  printf("TESTING %s\n", test_case.position);
  for (int i = test_case.depth_of_first_row;
       i <= test_case.depth_of_largest_row; i++) {
    load_position(&chess_state, test_case.position);
    perft_results_t got = perft(&chess_state, i, test_case.flags);
    if (!compare_perft_results(
            got, test_case.results_table[i - test_case.depth_of_first_row],
            test_case.flags)) {
      printf("\nFAILED DEPTH %d expected:\n\n", i);
      print_perft_results(
          test_case.results_table[i - test_case.depth_of_first_row],
          test_case.flags);
      printf("\ngot:\n\n");
      print_perft_results(got, test_case.flags);
    } else {
      printf("\nPASSED %d\n", i);
    }
  }
}

#include <string.h>
#include <stdlib.h>

int main(int argc, char const* argv[]) {
  if (argc != 2) {
    printf("usage: %s [file]\n", argv[0]);
    exit(0);
  }
  char buffer[1024];
  FILE* file = fopen(argv[1], "r");
  if (file == NULL) {
    perror("fopen failed");
    exit(1);
  }
  for (;;) {
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
      if (feof(file)) {
        break;
      }
      perror("fgets failed");
      exit(1);
    }
    char* arg = buffer;
    for (int i = 0; i < 6; i++) {
      arg = strchr(arg, ' ') + 1;
    }
    int draft = atoi(arg);
    arg = strchr(arg, ' ') + 1;
    int node_count = atoi(arg);

    perft_results_t results = {node_count};
    perft_test_t test =
        (perft_test_t){buffer, &results, draft, draft, PERFT_JUST_NODES};
    test_perft(test);
  }
  return 0;
}