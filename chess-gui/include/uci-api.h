#ifndef BOT_API_H
#define BOT_API_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "../../chess-lib/include/chess-lib.h"

typedef struct bot_term_cond_t {
  size_t time_limit_ms;
  size_t node_limit_nds;
  size_t depth_limit_ply;
  size_t mate_in_ply;
} bot_term_cond_t;

typedef struct match_state_t {
  // time control info
  size_t wtime;
  size_t btime;
  size_t winc;
  size_t binc;
} match_state_t;

typedef struct bot_interface_t {
  FILE *from_bot, *to_bot;  // streams for talking to bot
  pid_t pid;                // bot pid
  int quiet_mode;           // log all commands and responses to standard out
  const char *path;
  char *name;  // store info about bot
} bot_interface_t;

int create_bot(bot_interface_t* bot_iface, const char* path);
int kill_bot(bot_interface_t* bot_iface);

int uci_send(bot_interface_t* bot_iface, const char* cmd);

int uci_send_uci(bot_interface_t* bot_iface);
int uci_send_isready(bot_interface_t* bot_iface);
int uci_send_debug(bot_interface_t* bot_iface, int debug);
int uci_send_setoption(bot_interface_t* bot_iface, const char* text);
int uci_send_ucinewgame(bot_interface_t* bot_iface);
int uci_send_position(bot_interface_t* bot_iface, chess_state_t* chess_state);

int uci_send_go(bot_interface_t* bot_iface, move_t* searchmoves, int searchmovecount, int ponder, match_state_t* clockinfo, bot_term_cond_t* stopcond);
int uci_send_go_ponder(bot_interface_t* bot_iface, move_t* searchmoves, int searchmovecount, match_state_t* clockinfo, bot_term_cond_t* stopcond);
int uci_send_go_full(bot_interface_t* bot_iface, int ponder, match_state_t* clockinfo, bot_term_cond_t* stopcond);
int uci_send_go_full_ponder(bot_interface_t* bot_iface, match_state_t* clockinfo, bot_term_cond_t* stopcond);

int uci_send_stop(bot_interface_t* bot_iface);
int uci_send_ponderhit(bot_interface_t* bot_iface);
int uci_send_quit(bot_interface_t* bot_iface);

long uci_read_block(bot_interface_t* bot_iface, char* msg_buffer, long buffer_size);
long uci_read_noblock(bot_interface_t* bot_iface, char* msg_buffer, long buffer_size);

long uci_read_bestmove(bot_interface_t* bot_iface, chess_state_t* chess_state, move_t* bestmove, move_t* ponder);



#endif