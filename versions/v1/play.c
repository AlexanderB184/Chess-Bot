#include "interface.h"

// typedef Move(find_best_move_fn)(const Board *);
//
//   typedef enum {
//     OKAY,
//     GAMEOVER,
//     IO_FAILURE,
//     ILLEGAL_MOVE,
//     TIMEOUT,
//     INVALID_MESSAGE
//   } INTERFACE_EXIT_CODES;
//
//   INTERFACE_EXIT_CODES run_bot(FILE * in, FILE * out,
//                                find_best_move_fn * find_best_move);

;

enum OUTCOME { WHITE_WIN, BLACK_WIN, DRAW, ERROR };

enum OUTCOME play(find_best_move_fn *white_player,
                  find_best_move_fn *black_player) {
  // spawn two threads and run white and black on each.
  // write init message to eaches, input stream
  fprintf(stdout, "%s\n%s\n",
          "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "white");
  fprintf(stdout, "%s\n%s\n",
          "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "black");
}
