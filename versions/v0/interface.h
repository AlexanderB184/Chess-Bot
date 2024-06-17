#include <stdio.h>

#include "chess.h"
#include "fen.h"

typedef Move(find_best_move_fn)(const Board*);

typedef enum {
  OKAY,
  GAMEOVER,
  IO_FAILURE,
  ILLEGAL_MOVE,
  TIMEOUT,
  INVALID_MESSAGE
} INTERFACE_EXIT_CODES;

INTERFACE_EXIT_CODES run_bot(FILE* in, FILE* out,
                             find_best_move_fn* find_best_move);