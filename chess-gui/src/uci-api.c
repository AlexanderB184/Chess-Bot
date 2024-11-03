#include "../include/uci-api.h"

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>

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

void set_stdin(int* fd) {
  close(fd[1]);
  dup2(fd[0], STDIN_FILENO);
  close(fd[0]);
}

void set_stdout(int* fd) {
  close(fd[0]);
  dup2(fd[1], STDOUT_FILENO);
  dup2(fd[1], STDERR_FILENO);
  close(fd[1]);
}

int create_bot(bot_interface_t* bot_iface, const char* path) {
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
    set_stdin(in_fd);
    set_stdout(out_fd);
    char* argv[] = {NULL};
    execvp(path, argv);
    perror("execvp failed");
    exit(-1);
  }

  bot_iface->pid = fork_ret;

  close(in_fd[0]);
  close(out_fd[1]);

  bot_iface->to_bot = fdopen(in_fd[1], "w");
  bot_iface->from_bot = fdopen(out_fd[0], "r");

  if (!bot_iface->to_bot || !bot_iface->from_bot) {
    perror("fdopen failed");
    exit(-1);
  }

  bot_iface->path = path;
  bot_iface->name = path;

  return 0;
}

int kill_bot(bot_interface_t* bot_iface) {
  fclose(bot_iface->from_bot);
  fclose(bot_iface->to_bot);
  kill(bot_iface->pid, SIGTERM);
  return 0;
}

int uci_send(bot_interface_t* bot_iface, const char* cmd) {
  if (!bot_iface->quiet_mode) fprintf(stdout, "[GUI TO %s BOT] %s\n", bot_iface->name, cmd);
  if (fprintf(bot_iface->to_bot, "%s\n", cmd) < 0) {
    perror("send command failed");
    return -1;
  }
  fflush(bot_iface->to_bot);
  return 0;
}

int uci_send_uci(bot_interface_t* bot_iface) {
    return uci_send(bot_iface, "uci");
}

int uci_send_isready(bot_interface_t* bot_iface) {
    return uci_send(bot_iface, "isready");
}

int uci_send_debug(bot_interface_t* bot_iface, int debug) {
    return uci_send(bot_iface, debug ? "debug on" : "debug off");
}

int uci_send_setoption(bot_interface_t* bot_iface, const char* text);

int uci_send_ucinewgame(bot_interface_t* bot_iface) {
    return uci_send(bot_iface, "ucinewgame");
}

int uci_send_position(bot_interface_t* bot_iface, chess_state_t* chess_state) {
    // unmake to last irreversible move or root, save fen, then store each move after to move text
    char cmd[1024] = "position fen ";
    int idx = strlen(cmd);
    move_t move_stack[100];
    int ply_count = 0;

    while (chess_state->ply_counter > chess_state->ply_of_last_irreversible_move && chess_state->ply_counter>0) {
      move_stack[ply_count++] = chess_state->ply_stack[chess_state->ply_counter - 1].move;
      unmake_move(chess_state);
    }

    idx += save_position(chess_state, cmd + idx, sizeof(cmd) - idx);

    if (ply_count != 0) {
        cmd[idx++] = ' ';
        cmd[idx++] = 'm';
        cmd[idx++] = 'o';
        cmd[idx++] = 'v';
        cmd[idx++] = 'e';
        cmd[idx++] = 's';

        for (int i = ply_count-1; i >= 0; i--) {
            cmd[idx++] = ' ';
            make_move(chess_state, move_stack[i]);
            idx += write_long_algebraic_notation(cmd+idx, sizeof(cmd)-idx, move_stack[i]);
        }
    }


    return uci_send(bot_iface, cmd);
}

int uci_send_go(bot_interface_t* bot_iface, move_t* searchmoves, int searchmovecount, int ponder, match_state_t* clockinfo, bot_term_cond_t* stopcond) {
  char msg_buffer[1024] = "";
  long bytes_written = 0;
  bytes_written += snprintf(msg_buffer + bytes_written, sizeof(msg_buffer) - bytes_written, "go ");
  if (searchmoves) {

  }
  if (ponder) {
    bytes_written += snprintf(
      msg_buffer + bytes_written, sizeof(msg_buffer) - bytes_written, 
      "ponder "
    );
  }
  if (clockinfo) {
    bytes_written += snprintf(
      msg_buffer + bytes_written, sizeof(msg_buffer) - bytes_written, 
      "wtime %ld btime %ld winc %ld binc %ld ", 
      clockinfo->wtime, 
      clockinfo->btime, 
      clockinfo->winc, 
      clockinfo->binc
    );
  }
  if (stopcond) {
    if (stopcond->depth_limit_ply) {
      bytes_written += snprintf(
        msg_buffer + bytes_written, sizeof(msg_buffer) - bytes_written, 
        "depth %ld ", stopcond->depth_limit_ply
      );
    }
    if (stopcond->node_limit_nds) {
      bytes_written += snprintf(
        msg_buffer + bytes_written, sizeof(msg_buffer) - bytes_written, 
        "nodes %ld ", stopcond->node_limit_nds
      );
    }
    if (stopcond->time_limit_ms) {
      bytes_written += snprintf(
        msg_buffer + bytes_written, sizeof(msg_buffer) - bytes_written, 
        "movetime %ld ", stopcond->time_limit_ms
      );
    }
    if (stopcond->mate_in_ply) {
      bytes_written += snprintf(
        msg_buffer + bytes_written, sizeof(msg_buffer) - bytes_written, 
        "matein %ld ", stopcond->mate_in_ply
      );
    }
    if (!stopcond->depth_limit_ply && !stopcond->node_limit_nds
     && !stopcond->time_limit_ms   && !stopcond->mate_in_ply) {
      bytes_written += snprintf(
        msg_buffer + bytes_written, sizeof(msg_buffer) - bytes_written, 
        "infinite "
      );
    }
  }

  return uci_send(bot_iface, msg_buffer);
}

int uci_send_stop(bot_interface_t* bot_iface) {
    return uci_send(bot_iface, "stop");
}
int uci_send_ponderhit(bot_interface_t* bot_iface) {
    return uci_send(bot_iface, "ponderhit");
}
int uci_send_quit(bot_interface_t* bot_iface) {
    return uci_send(bot_iface, "quite");
}

long uci_read_block(bot_interface_t* bot_iface, char* msg_buffer, long buffer_size) {
  if (fgets(msg_buffer, buffer_size, bot_iface->from_bot) == NULL) {
  if (feof(bot_iface->from_bot)) {
    return -1;
  }
  perror("read response failed");
  exit(-1);
  }
  if (!bot_iface->quiet_mode) fprintf(stdout, "[BOT %s TO GUI] %s", bot_iface->name, msg_buffer);
  return strlen(msg_buffer);
}

long uci_read_noblock(bot_interface_t* bot_iface, char* msg_buffer, long buffer_size);

long uci_read_bestmove(bot_interface_t* bot_iface, chess_state_t* position, move_t* bestmove, move_t* ponder) {
    char msg_buffer[1024];
    do {
    if (uci_read_block(bot_iface, msg_buffer, sizeof(msg_buffer)) < 0) {
      return 0;
    }
    if (strcmp(msg_buffer, "bestmove") == 0) {
      long bytes_left = sizeof(msg_buffer);
      read_long_algebraic_notation(msg_buffer, bytes_left, position, bestmove);
      // check if ponder token exists
      read_long_algebraic_notation(msg_buffer, bytes_left, position, ponder);
    }
  } while (1);
}
