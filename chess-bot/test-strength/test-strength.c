#include "runbot.h"


char buffer[1024];
char moves[512];


chess_state_t match;

int main(int argc, const char** argv) {
    if (argc != 3) {
        printf("usage: %s [baseline path] [comparison path]\n", argv[0]);
        exit(0);
    }
    bot_interface_t bot[2];
    spawn_bot(argv[1], &bot[0]);
    send_command(&bot[0], "uci");
    wait_response(&bot[0], "uciok", buffer, sizeof(buffer));

    spawn_bot(argv[2], &bot[1]);
    send_command(&bot[1], "uci");
    wait_response(&bot[1], "uciok", buffer, sizeof(buffer));

    load_start_position(&match);
    int turn = 0;
    do {
        SEND(&bot[turn], "position startpos moves %s", moves);
        send_command(&bot[turn], "isready");
        wait_response(&bot[turn], "readyok", buffer, sizeof(buffer));
        send_command(&bot[turn], "go movetime 100");
        wait_response(&bot[turn], "bestmove", buffer, sizeof(buffer));
        add_move(moves, sizeof(moves), &match, buffer);
        turn = turn ? 0 : 1;

    } while (!is_gameover(&match));
    if (is_checkmate(&match)) {
        fprintf(stdout, "%s won\n", bot[turn ? 0 : 1].name);
    } else {
        fprintf(stdout, "draw\n");
    }
    write_pgn(buffer, sizeof(buffer), &match, NULL, NULL, NULL, NULL, bot[0].name, bot[1].name, NULL);
    fprintf(stdout, "%s\n", buffer);
    send_command(&bot[0], "quit");
    send_command(&bot[1], "quit");

    
}