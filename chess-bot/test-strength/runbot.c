#include "runbot.h"

int spawn_bot(const char* bot_path, bot_interface_t* bot_iface) {
  int in_fd[2];
  int out_fd[2];
  if (pipe(in_fd) == -1 || pipe(out_fd) == -1) {
    perror("pipe failed");
    exit(-1);
  }

  int fork_ret = fork();
  if (fork_ret == -1) {
    perror("fork failed");
    exit(-1);
  }

  if (fork_ret == 0) {
    close(in_fd[1]);
    dup2(in_fd[0], STDIN_FILENO);
    close(in_fd[0]);

    close(out_fd[0]);
    dup2(out_fd[1], STDOUT_FILENO);
    // dup2(out_fd[1], STDERR_FILENO);
    close(out_fd[1]);
    char* argv[] = {NULL};
    execvp(bot_path, argv);
    perror("execvp failed");
    exit(-1);
  }

  close(in_fd[0]);
  close(out_fd[1]);

  bot_iface->pid = fork_ret;

  bot_iface->to_bot = fdopen(in_fd[1], "w");
  bot_iface->from_bot = fdopen(out_fd[0], "r");

  if (!bot_iface->to_bot || !bot_iface->from_bot) {
    perror("fdopen failed");
    exit(-1);
  }

  bot_iface->name = bot_path;

  return fork_ret;
}

int kill_bot(bot_interface_t* bot_iface) {
  fclose(bot_iface->from_bot);
  fclose(bot_iface->to_bot);
  kill(bot_iface->pid, SIGTERM);
  return 0;
}

int send_command(bot_interface_t* bot_iface, const char* cmd) {
  //char buffer[256];
  fprintf(stdout, "[GUI TO %s BOT] %s\n", bot_iface->name, cmd);
  if (fprintf(bot_iface->to_bot, "%s\n", cmd) < 0) {
    perror("send command failed");
  }
  fflush(bot_iface->to_bot);
  return 1;
}

char* read_response(bot_interface_t* bot_iface, char* buffer, size_t size) {
  if (fgets(buffer, size, bot_iface->from_bot) == NULL) {
    if (feof(bot_iface->from_bot)) {
      return NULL;
    }
    perror("read response failed");
    exit(-1);
  }
  fprintf(stdout, "[BOT %s TO GUI] %s", bot_iface->name, buffer);
  return buffer;
}

// waits for a response that begins with the token "token", ignores all other
// responses
int wait_response(bot_interface_t* bot_iface, const char* token, char* buffer,
                  size_t size) {
  int toksize = strlen(token);
  do {
    if (read_response(bot_iface, buffer, size) == NULL) {
      return 0;
    }
    if (strncmp(buffer, token, toksize) == 0) {
      return 1;
    }
  } while (1);
}

void add_move(char* movetext, size_t space, chess_state_t* match, char* response) {
  // looks like 'bestmove XXXX ponder XXXX\n'
  // ponder is optional
  char* line = response;
  next_arg(&line);               // extract bestmove token
  char* move = next_arg(&line);  // get move
  move_t imr;
  if (read_long_algebraic_notation(move, strlen(move), match, &imr) < 0) {
    fprintf(stderr, "ILLEGAL MOVE PLAYED!!!\n");
    exit(0);
  }
  make_move(match, imr);
  strcat(movetext, " ");
  strcat(movetext, move);
}
char* next_arg(char** line_ptr) {
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
