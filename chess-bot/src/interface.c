#include <ctype.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/bot.h"
#include "../include/search.h"

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 0
#endif

#define AUTHOR "Alex B"
#define BOT_NAME "ABBOT"
#define BOT_VERSION "0.0"

#define UNIMPLEMENTED fprintf(stderr, "unimplemented\n")
#define INVALIDARG(CMD, ARG) fprintf(stderr, "\"%s\" is not a valid argument for command \"%s\"\n", (ARG), (CMD))
#define MISSINGARG(CMD, ARG) fprintf(stderr, "command \"%s\" is missing required argument \"%s\"\n", (ARG), (CMD))

#define MAX_ARGS 64

char * next_arg(char** line_ptr) {
    char* line = *line_ptr;
    while (*line && isspace(*line)) {
        line++;
    }

    if (*line == '\0') {
        *line_ptr = line;
        return NULL;
    }

    char* arg_start = line;

    while (*line && !isspace(*line)) {
        line++;
    }

    if (*line) {
        *line = '\0';
        line++;
    }

    while (*line && isspace(*line)) {
        line++;
    }

    *line_ptr = line;

    return arg_start;
}

bot_t bot;
chess_state_t root_pos;

int main(int argc, const char *argv) {
  {
    bot_settings_t settings = {
        .nthreads = 1,
        .log_stream = stdout,
        .err_stream = stderr,
    };
    if (bot_init(&bot, &settings)) {
      fprintf(stderr,
              "failed to initialise bot " BOT_NAME " " BOT_VERSION "\n");
      exit(-1);
    }
  }
  char buffer[256];
  do {
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        if (feof(stdin)) {
            break;  // End of input
        }
      perror("fgets failed");
      exit(-1);
    }
    char* line = buffer;
    char* cmd = next_arg(&line);
    if (cmd == NULL) {
      continue;
    }
    if (strcmp(cmd, "uci") == 0) {
      fprintf(stdout, "id name " BOT_NAME " " BOT_VERSION "\n");
      fprintf(stdout, "id author " AUTHOR "\n");
      fprintf(stdout, "uciok\n");
    } else if (strcmp(cmd, "debug") == 0) {
      UNIMPLEMENTED;
    } else if (strcmp(cmd, "isready") == 0) {
      fprintf(stdout, "readyok\n");
    } else if (strcmp(cmd, "setoption") == 0) {
      UNIMPLEMENTED;
    } else if (strcmp(cmd, "register") == 0) {
      UNIMPLEMENTED;
    } else if (strcmp(cmd, "ucinewgame") == 0) {
      bot_load_fen(&bot, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    } else if (strcmp(cmd, "position") == 0) {
      char * arg = next_arg(&line);
      if (arg && strcmp(arg, "startpos") == 0) {
        bot_load_fen(&bot, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
      } else if (arg && strcmp(arg, "fen") == 0) {
        int out = bot_load_fen(&bot, line);
        if (out < 0) {
          fprintf(stderr, "invalid fen \"%s\"\n", line);
          continue;
        }
      }
      arg = next_arg(&line);
      if (arg && strcmp(arg, "moves") == 0) {
        if (bot_load_moves(&bot, line) < 0) {
          fprintf(stderr, "invalid move in movetext \"%s\"\n", line);
          continue;
        }
      } else {
        MISSINGARG("position", "moves");
      }
    } else if (strcmp(cmd, "go") == 0) {
      int i = 1;
      bot_term_cond_t stop_cond;
      char * arg = next_arg(&line);
      while (arg) {
        if (strcmp(arg, "searchmoves") == 0) {
          UNIMPLEMENTED;
        } else if (strcmp(arg, "ponder") == 0) {
          UNIMPLEMENTED;
          arg = next_arg(&line);
        } else if (strcmp(arg, "wtime") == 0) {
          UNIMPLEMENTED;
          arg = next_arg(&line);
        } else if (strcmp(arg, "btime") == 0) {
          UNIMPLEMENTED;
          arg = next_arg(&line);
        } else if (strcmp(arg, "winc") == 0) {
          UNIMPLEMENTED;
          arg = next_arg(&line);
        } else if (strcmp(arg, "binc") == 0) {
          UNIMPLEMENTED;
          arg = next_arg(&line);
        } else if (strcmp(arg, "movestogo") == 0) {
          UNIMPLEMENTED;
          arg = next_arg(&line);
        } else if (strcmp(arg, "depth") == 0) {
          arg = next_arg(&line);
          stop_cond.depth_limit_ply = atoll(arg);
        } else if (strcmp(arg, "nodes") == 0) {
          arg = next_arg(&line);
          stop_cond.node_limit_nds = atoll(arg);
        } else if (strcmp(arg, "mate") == 0) {
          arg = next_arg(&line);
          stop_cond.depth_limit_ply = atoll(arg);
        } else if (strcmp(arg, "movetime") == 0) {
          arg = next_arg(&line);
          stop_cond.time_limit_ms = atoll(arg);
        } else if (strcmp(arg, "infinite") == 0) {
          stop_cond.depth_limit_ply = 0;
          stop_cond.node_limit_nds = 0;
          stop_cond.time_limit_ms = 0;
        } else {
          INVALIDARG(cmd, arg);
        }
        arg = next_arg(&line);
      }
      bot_start(&bot, &stop_cond);
    } else if (strcmp(cmd, "stop") == 0) {
      bot_stop(&bot);
    } else if (strcmp(cmd, "ponderhit") == 0) {
      UNIMPLEMENTED;
    } else if (strcmp(cmd, "quit") == 0) {
      bot_release(&bot);
      exit(0);
    }
  } while (1);
  bot_release(&bot);
}