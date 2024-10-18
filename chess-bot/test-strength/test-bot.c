#include "runbot.h"

char buffer[1028];
char moves[512];


int main(int argc, const char** argv) {
    if (argc != 2) {
        printf("usage: %s [bot path]\n", argv[0]);
        exit(0);
    }
    bot_interface_t bot;
    spawn_bot(argv[1], &bot);

    send_command(&bot, "uci");
    wait_response(&bot, "uciok", buffer, sizeof(buffer));
    send_command(&bot, "position startpos moves d2d4 e7e5 c2c4 e5d4 d1d4 b8c6 d4c3");
    send_command(&bot, "isready");
    wait_response(&bot, "readyok", buffer, sizeof(buffer));
    send_command(&bot, "go depth 7");
    wait_response(&bot, "bestmove", buffer, sizeof(buffer));
    /*add_move(moves, sizeof(moves), buffer);
    sprintf(buffer, "position startpos moves %s", moves);
    send_command(&bot, buffer);
    send_command(&bot, "go depth 8");
    wait_response(&bot, "bestmove", buffer, sizeof(buffer));
*/
    send_command(&bot, "quit");

    
}