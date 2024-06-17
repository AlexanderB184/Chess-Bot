#include <dlfcn.h>

#include "bots/alpha_beta.h"
#include "bots/debug.h"
#include "bots/id.h"
#include "bots/minmax.h"
#include "chess.h"
#include "chess_gui.h"

struct debug_info *dbg = NULL;

typedef size_t milliseconds_t;

typedef void(init_bot_fn_t)(void *);
typedef Move(find_best_move_fn_t)(const Board *, milliseconds_t);

typedef struct Bot_t {
  const char *name;
  init_bot_fn_t *init_bot;
  find_best_move_fn_t *find_best_move;
  void *handle;
} Bot_t;

typedef struct Results {
  int wins, losses, draws;
} Results;

static struct debug_info bot_stats[2];

int play_game(Bot_t p1, Bot_t p2) {
  Board board = start_board;
  loop {
    Move mv = p1.find_best_move(&board, 100);
    make_move(&board, mv);
    // display_board(stdout,&board, board.turn);

    if (is_stalemate(&board)) {
      printf("game finished\n");
      if (is_check(&board)) {
        return 1;
      } else {
        return 0;
      }
    }
    mv = p2.find_best_move(&board, 100);
    make_move(&board, mv);
    // display_board(stdout,&board, board.turn);

    if (is_stalemate(&board)) {
      printf("game finished\n");
      if (is_check(&board)) {
        return -1;
      } else {
        return 0;
      }
    }
    // printf("%u\n", board.move_count);
  }
}

Results play_games(Bot_t p1, Bot_t p2, int games) {
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

const static int games = 1;

void load_bot(Bot_t *bot, const char *lib_path, const char *init_fn,
              const char *move_fn) {
  bot->handle = dlopen(lib_path, RTLD_LAZY);
  if (!bot->handle) {
    fprintf(stderr, "Cannot open library: %s\n", dlerror());
    exit(1);
  }
  bot->init_bot = (init_bot_fn_t *)dlsym(bot->handle, init_fn);
  if (!bot->init_bot) {
    fprintf(stderr, "Cannot load symbol '%s': %s\n", init_fn, dlerror());
    dlclose(bot->handle);
    exit(1);
  }
  bot->find_best_move = (find_best_move_fn_t *)dlsym(bot->handle, move_fn);
  if (!bot->find_best_move) {
    fprintf(stderr, "Cannot load symbol '%s': %s\n", move_fn, dlerror());
    dlclose(bot->handle);
    exit(1);
  }
}

void unload_bot(Bot_t *bot) {
  if (bot->handle) {
    dlclose(bot->handle);
  }
}

int main(int argc, char const *argv[]) {
  static Bot_t bots[2];
  static int win_table[lengthof(bots)][lengthof(bots)];
  static int loss_table[lengthof(bots)][lengthof(bots)];
  static int draw_table[lengthof(bots)][lengthof(bots)];

  load_bot(&bots[0], "./versions/bot_v0.so", "init_bot", "best_move");
  load_bot(&bots[1], "./versions/bot_v1.so", "init_bot", "best_move");
  bots[0].init_bot(&bot_stats[0]);
  bots[1].init_bot(&bot_stats[1]);
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
  (void)draw_table;
  (void)loss_table;

  for (int i = 0; i < lengthof(bots); i++) {
    for (int j = 0; j < lengthof(bots); j++) {
      printf("%f ", (float)win_table[i][j] / (float)games);
    }
    printf("\n");
  }
  dbg_display(0, bot_stats[0]);
  dbg_display(1, bot_stats[1]);

  unload_bot(&bots[0]);
  unload_bot(&bots[1]);

  return 0;
}
