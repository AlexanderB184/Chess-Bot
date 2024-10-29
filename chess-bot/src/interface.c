#include <ctype.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "../include/bot.h"
#include "../include/search.h"

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 0
#endif

#define AUTHOR "Alex B"
#define BOT_NAME "ABBOT"
#define BOT_VERSION "0.4"

#define UNIMPLEMENTED fprintf(stderr, "unimplemented\n")
#define INVALIDARG(CMD, ARG) fprintf(stderr, "\"%s\" is not a valid argument for command \"%s\"\n", (ARG), (CMD))
#define MISSINGARG(CMD, ARG) fprintf(stderr, "command \"%s\" is missing required argument \"%s\"\n", (CMD), (ARG))
#define LOGERROR(msg, args...) fprintf(stderr, msg "\n", ##args)
#define FATALERROR(msg, args...) {LOGERROR(msg, ##args); exit(-1);}
#define TELLGUI(msg, args...) { fprintf(stdout, msg "\n", ##args); fflush(stdout); }

#define BUFFER_SIZE 1 << 12

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

const char * startpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

void uci_handler(char* line) {
  if (bot_init(&bot, NULL)) {
    FATALERROR("failed to initialise bot " BOT_NAME " " BOT_VERSION);
  }
  TELLGUI("id name " BOT_NAME " " BOT_VERSION);
  TELLGUI("id author " AUTHOR);
  TELLGUI("uciok");
}

void debug_handler(char* line) {
  char * arg = next_arg(&line);
  if (arg && strcmp(arg, "on") == 0) {
    bot.settings.debug = 1;
  } else if (arg && strcmp(arg, "off") == 0) {
    bot.settings.debug = 0;
  } else {
    MISSINGARG("debug", "on/off");
  }
}

void isready_handler(char* line) {
  TELLGUI("readyok");

}

void ucinewgame_handler(char* line) {
  bot_load_fen(&bot, startpos);

}

void position_handler(char* line) {
  char * arg = next_arg(&line);
  if (arg && strcmp(arg, "startpos") == 0) {
    bot_load_fen(&bot, startpos);
  } else if (arg && strcmp(arg, "fen") == 0) {
    int out = bot_load_fen(&bot, line);
    if (out < 0) {
      LOGERROR("invalid fen \"%s\"", line);
      return;
    }
    line += out;
  }
  arg = next_arg(&line);
  if (arg && strcmp(arg, "moves") == 0) {
    if (bot_load_moves(&bot, line) < 0) {
      LOGERROR("invalid move in movetext \"%s\"", line);
      return;
    }
  } else {
    MISSINGARG("position", "moves");
  }
}

void go_handler(char* line) {
  bot.search_mode = SEARCHMODE_REGULAR;
  bot.stop_cond = (bot_term_cond_t){0, 0, 0, 0};
  char * arg = next_arg(&line);
  while (arg) {
    if (strcmp(arg, "searchmoves") == 0) {
      UNIMPLEMENTED;
    } else if (strcmp(arg, "ponder") == 0) {
      if (bot.settings.Ponder != 1) {
        LOGERROR("cannot ponder, ponder mode is disabled");
        return; // do not ponder
      } else {
        bot.search_mode = SEARCHMODE_PONDER;
      }
    } else if (strcmp(arg, "wtime") == 0) {
      
      arg = next_arg(&line);
      if (arg) {
      bot.match_info.wtime = atoll(arg);
      } else {
        MISSINGARG("wtime", "<time ms>");
      }
    } else if (strcmp(arg, "btime") == 0) {
      arg = next_arg(&line);
      if (arg) {
        bot.match_info.btime = atoll(arg);
      } else {
        MISSINGARG("btime", "<time ms>");
      }
    } else if (strcmp(arg, "winc") == 0) {
      arg = next_arg(&line);
      if (arg) {
        bot.match_info.winc = atoll(arg);
      } else {
        MISSINGARG("winc", "<inc ms>");
      }
    } else if (strcmp(arg, "binc") == 0) {
      arg = next_arg(&line);
      if (arg) {
        bot.match_info.binc = atoll(arg);
      } else {
        MISSINGARG("binc", "<inc ms>");
      }
    } else if (strcmp(arg, "movestogo") == 0) {
      UNIMPLEMENTED;
      arg = next_arg(&line);
    } else if (strcmp(arg, "depth") == 0) {
      arg = next_arg(&line);
      if (arg) {
        bot.stop_cond.depth_limit_ply = atoll(arg);
      }
    } else if (strcmp(arg, "nodes") == 0) {
      arg = next_arg(&line);
      if (arg) {
        bot.stop_cond.node_limit_nds = atoll(arg);
      }
    } else if (strcmp(arg, "mate") == 0) {
      arg = next_arg(&line);
      if (arg) {
        bot.stop_cond.mate_in_ply = atoll(arg);
      }
    } else if (strcmp(arg, "movetime") == 0) {
      arg = next_arg(&line);
      if (arg) {
        bot.stop_cond.time_limit_ms = atoll(arg);
      }
    } else if (strcmp(arg, "infinite") == 0) {
      bot.stop_cond.depth_limit_ply = 0;
      bot.stop_cond.node_limit_nds = 0;
      bot.stop_cond.time_limit_ms = 0;
      bot.stop_cond.mate_in_ply = 0;
    } else {
      INVALIDARG("go", arg);
    }
    arg = next_arg(&line);
  }

  bot_start(&bot);
}

void stop_handler(char* line) {
  bot_stop(&bot);
}

void ponderhit_handler(char* line) {
  bot_ponder_hit(&bot);
}

void quit_handler(char* line) {
  bot_release(&bot);
  exit(0);
}

void fetch_command(char* buffer, long size) {
  if (fgets(buffer, size, stdin) == NULL) {
    if (feof(stdin)) {
      LOGERROR("bot reached eof");
      quit_handler(NULL);
    }
    FATALERROR("bot failed to read from stdin: %s", strerror(errno));
  }
}

void handle_command(char* line) {
  
  int invalid_cmd;

  do {

    char* cmd = next_arg(&line);

    if (cmd == NULL) {
      break;
    }

    invalid_cmd = 0;

    if (strcmp(cmd, "uci") == 0) {
      uci_handler(line);

    } else if (strcmp(cmd, "debug") == 0) {
      debug_handler(line);

    } else if (strcmp(cmd, "isready") == 0) {
      isready_handler(line);

    } else if (strcmp(cmd, "setoption") == 0) {
      UNIMPLEMENTED;

    } else if (strcmp(cmd, "register") == 0) {
      UNIMPLEMENTED;

    } else if (strcmp(cmd, "ucinewgame") == 0) {
      ucinewgame_handler(line);

    } else if (strcmp(cmd, "position") == 0) {
      position_handler(line);

    } else if (strcmp(cmd, "go") == 0) {
      go_handler(line);
      
    } else if (strcmp(cmd, "stop") == 0) {
      stop_handler(line);

    } else if (strcmp(cmd, "ponderhit") == 0) {
      ponderhit_handler(line);

    } else if (strcmp(cmd, "quit") == 0) {
      quit_handler(line);

    } else {
      LOGERROR("\"%s\" is not a valid command", cmd);
      invalid_cmd = 1;
    }

  } while (invalid_cmd); // loop until we find a valid command

}

int main(int argc, char **argv) {
  char buffer[BUFFER_SIZE] = {};
  for (;;) {
    fetch_command(buffer, BUFFER_SIZE);
    handle_command(buffer);
  }
}