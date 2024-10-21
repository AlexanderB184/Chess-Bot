#include "runbot.h"

char buffer[3000];
char moves[3000] = "";

int quiet_mode = 0;

chess_state_t match = {};

int main(int argc, const char** argv) {
    if (argc != 4) {
        printf("usage: %s [baseline path] [comparison path] [number of games]\n", argv[0]);
        exit(0);
    }

    int num_games = atoi(argv[3]);
    if (num_games <= 0) {
        printf("Please provide a valid number of games\n");
        exit(1);
    }

    bot_interface_t bot[2];
    spawn_bot(argv[1], &bot[0]);  // Bot 0
    send_command(&bot[0], "uci");
    wait_response(&bot[0], "uciok", buffer, sizeof(buffer));

    spawn_bot(argv[2], &bot[1]);  // Bot 1
    send_command(&bot[1], "uci");
    wait_response(&bot[1], "uciok", buffer, sizeof(buffer));

    int wins[2] = {0, 0};  // wins[0] for bot[0], wins[1] for bot[1]
    int draws = 0;

    for (int game = 0; game < num_games; game++) {
        memset(moves, 0, sizeof(moves));  // Clear the moves for each game
        load_position(&match, "r1bqkb1r/pppp1ppp/2n2n2/4p1N1/2B1P3/8/PPPP1PPP/RNBQK2R b KQkq - 5 4");      // Reset the game state
        int turn = game % 2;              // Alternate which bot starts as white
        int winner = -1;                  // -1 means no winner (for draws)

        do {
            SEND(&bot[turn], "position fen %s moves %s", "r1bqkb1r/pppp1ppp/2n2n2/4p1N1/2B1P3/8/PPPP1PPP/RNBQK2R b KQkq - 5 4", moves);
            send_command(&bot[turn], "isready");
            wait_response(&bot[turn], "readyok", buffer, sizeof(buffer));
            send_command(&bot[turn], "go movetime 100");
            wait_response(&bot[turn], "bestmove", buffer, sizeof(buffer));
            add_move(moves, sizeof(moves), &match, buffer);
            turn = turn ? 0 : 1;

        } while (!is_gameover(&match));

        if (is_checkmate(&match)) {
            winner = turn ? 0 : 1;  // If bot[turn] just played, other bot won
            wins[winner]++;
            fprintf(stdout, "%s won game %d\n", bot[winner].name, game + 1);
        } else {
            draws++;
            fprintf(stdout, "Game %d ended in a draw\n", game + 1);
        }

        // Optionally, write PGN after each game
        //write_pgn(buffer, sizeof(buffer), &match, NULL, NULL, NULL, NULL, bot[0].name, bot[1].name, NULL);
        //fprintf(stdout, "Game %d PGN:\n%s\n", game + 1, buffer);
    }

    // Summary of results
    fprintf(stdout, "Results after %d games:\n", num_games);
    fprintf(stdout, "%s: %d wins\n", bot[0].name, wins[0]);
    fprintf(stdout, "%s: %d wins\n", bot[1].name, wins[1]);
    fprintf(stdout, "Draws: %d\n", draws);

    // Send quit command to the bots
    send_command(&bot[0], "quit");
    send_command(&bot[1], "quit");

    return 0;
}