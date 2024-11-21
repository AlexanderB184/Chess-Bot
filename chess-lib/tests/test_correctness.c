#include <stdio.h>

#include "../include/chess-lib.h"
#include "../include/perft.h"

static chess_state_t chess_state;
static chess_state_t chess_state2;

perft_results_t default_board_results[10] = {
    {1, 0, 0, 0, 0, 0, 0, 0, 0},
    {20, 0, 0, 0, 0, 0, 0, 0, 0},
    {400, 0, 0, 0, 0, 0, 0, 0, 0},
    {8902, 34, 0, 0, 0, 12, 0, 0, 0},
    {197281, 1576, 0, 0, 0, 469, 0, 0, 8},
    {4865609, 82719, 258, 0, 0, 27351, 6, 0, 347},
    {119060324, 2812008, 5248, 0, 0, 809099, 329, 46, 10828},
    {3195901860, 108329926, 319617, 883453, 0, 33103848, 18026, 1628, 435767},
    {84998978956, 3523740106, 7187977, 23605205, 0, 968981593, 847039, 147215,
     9852036},
    {2439530234167, 125208536153, 319496827, 1784356000, 17334376, 36095901903,
     37101713, 5547231, 400191963},
};

perft_results_t Kiwipete_results[7] = {
    {48, 8, 0, 2, 0, 0, 0, 0, 0},
    {2039, 351, 1, 91, 0, 3, 0, 0, 0},
    {97862, 17102, 45, 3162, 0, 993, 0, 0, 1},
    {4085603, 757163, 1929, 128013, 15172, 25523, 42, 6, 43},
    {193690690, 35043416, 73365, 4993637, 8392, 3309887, 19883, 2645, 30171},
    {8031647685, 1558445089, 3577504, 184513607, 56627920, 92238050, 568417,
     54948, 360003},
};

perft_results_t position_3_results[9] = {
    {14, 1, 0, 0, 0, 2, 0, 0, 0},
    {191, 14, 0, 0, 0, 10, 0, 0, 0},
    {2812, 209, 2, 0, 0, 267, 3, 0, 0},
    {43238, 3348, 123, 0, 0, 1680, 106, 0, 17},
    {674624, 52051, 1165, 0, 0, 52950, 1292, 3, 0},
    {11030083, 940350, 33325, 0, 7552, 452473, 26067, 0, 2733},
    {178633661, 14519036, 294874, 0, 140024, 12797406, 370630, 3612, 87},
    {3009794393, 267586558, 8009239, 0, 6578076, 135626805, 7181487, 1630,
     450410},
};

perft_results_t position_4_results[9] = {
    {6, 0, 0, 0, 0, 0, 0, 0, 0},
    {264, 87, 0, 6, 48, 10, 0, 0, 0},
    {9467, 1021, 4, 0, 120, 38, 0, 0, 22},
    {422333, 131393, 0, 7795, 60032, 15492, 0, 0, 5},
    {15833292, 2046173, 6512, 0, 329464, 200568, 0, 0, 50562},
    {706045033, 210369132, 212, 10882006, 81102984, 26973664, 0, 0, 81076},
};

perft_results_t position_5_results[5] = {
    {44}, {1486}, {62379}, {2103487}, {89941194}};

const char* positions[] = {
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
    "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
    "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"};

const perft_results_t* results_tables[] = {default_board_results,
                                           Kiwipete_results, position_3_results,position_4_results,
                                           position_5_results};

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
    load_position(&chess_state2, test_case.position);
    copy_position(&chess_state, &chess_state2);
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

int main(int argc, char const* argv[]) {
  const perft_test_t test_cases[] = {
      {positions[0], results_tables[0], 0, 5, PERFT_ALL},
      {positions[1], results_tables[1], 1, 5, PERFT_ALL},
      {positions[2], results_tables[2], 1, 5, PERFT_ALL},
      {positions[3], results_tables[3], 1, 5, PERFT_NODES|PERFT_CAPTURES|PERFT_CASTLES|PERFT_ENPASSENT|PERFT_CHECKS|PERFT_CHECKMATE|PERFT_PROMOTIONS},
      {positions[4], results_tables[4], 1, 5, PERFT_JUST_NODES},
  };

  test_perft(test_cases[0]);
  test_perft(test_cases[1]);
  test_perft(test_cases[2]);
  test_perft(test_cases[3]);
  test_perft(test_cases[4]);

  return 0;
}