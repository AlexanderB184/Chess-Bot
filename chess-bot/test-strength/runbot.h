#ifndef RUNBOT_H
#define RUNBOT_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>

#include "../../chess-lib/include/chess-lib.h"

typedef struct {
    pid_t pid;
    FILE* to_bot;
    FILE* from_bot;
    const char* name;
} bot_interface_t;

int spawn_bot(const char * bot_path, bot_interface_t *bot_iface);

int kill_bot(bot_interface_t *bot_iface);

int send_command(bot_interface_t* bot_iface, const char * cmd);

#define SEND(BOT, CMD, ARGS...) { char CMDBUF[1024]; sprintf(CMDBUF, CMD, ##ARGS); send_command(BOT, CMDBUF); }

char * read_response(bot_interface_t* bot_iface, char * buffer, size_t size);

// waits for a response beggining with token, once recieved stores the full line in buffer.
int wait_response(bot_interface_t* bot_iface, const char* token, char* buffer, size_t size);

char * next_arg(char** line);

void add_move(char* movetext, size_t space, chess_state_t* match, char* move);
#endif