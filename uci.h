#include <stddef.h>

#include "chess.h"
#include "mylib/String.h"

typedef struct bot_state_t bot_state_t;
typedef struct interface_t interface_t;

typedef int(handler_t)(interface_t, bot_state_t*, Slice(String));

struct bot_state_t {
  Piece_Colour playing_as;
  Board board;
  // ... others
};

struct interface_t {
  FILE *input_stream, *output_stream;
};

handler_t handle;
