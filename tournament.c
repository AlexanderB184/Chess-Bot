#include "bots/minmax.h"
#include "bots/alpha_beta.h"
#include "bots/id.h"
#include "chess.h"
#include "chess_gui.h"
#include "bots/debug.h"

dbg_header();

typedef size_t milliseconds_t;

typedef Move (*Bot)(const Board *, milliseconds_t);

Bot bots[] = {best_move_id, best_move_id_new};

typedef struct Results {
  int wins, losses, draws;
} Results;

int play_game(Bot p1, Bot p2) {
  Board board = start_board;
  loop {
    Move mv = p1(&board, 100);
    make_move(&board, mv);
    //display_board(stdout,&board, board.turn);

    if (is_stalemate(&board)) {
        printf("game finished\n");
      if (is_check(&board)) {
        return 1;
      } else {
        return 0;
      }
    }
    mv = p2(&board, 100);
    make_move(&board, mv);
    //display_board(stdout,&board, board.turn);

    if (is_stalemate(&board)) {
      printf("game finished\n");
      if (is_check(&board)) {
        return -1;
      } else {
        return 0;
      }
    }
    //printf("%u\n", board.move_count);
  }
}

Results play_games(Bot p1, Bot p2, int games) {
  Results r = {0};
  for (int i = 0; i < games / 2; i++) {
    int res = play_game(p1, p2);
    if (res > 0) {
      r.wins++;
    } else if (res < 0) {
      r.losses++;
    } else {
      r.draws++;
    }
  }
  for (int i = games / 2; i < games; i++) {
    int res = play_game(p2, p1);
    if (res > 0) {
      r.losses++;
    } else if (res < 0) {
      r.wins++;
    } else {
      r.draws++;
    }
  }
  return r;
}

int win_table[lengthof(bots)][lengthof(bots)];
int loss_table[lengthof(bots)][lengthof(bots)];
int draw_table[lengthof(bots)][lengthof(bots)];

const static int games = 100;

int main(int argc, char const *argv[]) {
  (void)active_bot;

  init_id();
  for (int i = 0; i < lengthof(bots); i++) {
    for (int j = 0; j <= i; j++) {
      Results r = play_games(bots[i], bots[j], games);
      win_table[i][j] = r.wins;
      loss_table[i][j] = r.losses;
      draw_table[i][j] = r.draws;
      win_table[j][i] = r.losses;
      loss_table[j][i] = r.wins;
      draw_table[j][i] = r.draws;
    }
  }
  for (int i = 0; i < lengthof(bots); i++) {
    for (int j = 0; j <  lengthof(bots); j++) {
      printf("%f ", (float)win_table[i][j] / (float)games);
    }
    printf("\n");
  }
  dbg_display(0);
  dbg_display(1);
  return 0;
}
