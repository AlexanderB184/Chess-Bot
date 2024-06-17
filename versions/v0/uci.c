#include "uci.h"

#include <assert.h>

typedef struct handler_key_value_t handler_key_value_t;

struct handler_key_value_t {
  size_t hash;
  String cmd;
  handler_t* fn;
};

#define HANDLER_BUCKET_COUNT 19

static handler_key_value_t handlers[HANDLER_BUCKET_COUNT] = {0};

size_t uci_handler_hash_fn(String str) { return 0ull; }

int handle(interface_t interface, bot_state_t* state, String msg) {
  // split first token
  size_t cmd_end = find_char(msg, ' ');
  String cmd = substring(msg, 0, cmd_end);
  String arg = substring(msg, cmd_end + 1, msg.length);

  size_t hash = uci_handler_hash_fn(cmd);
  size_t index = hash % HANDLER_BUCKET_COUNT;
  handler_key_value_t handler = handlers[index];
  if (hash == handler.hash && string_compare(cmd, handler.cmd)) {
    return handler.fn(interface, state, arg);
  }
  return 0;
}

#define handler_for(cmd)                                                \
  int cmd##_handler(interface_t interface, volatile bot_state_t* state, \
                    String argument)

int uci_handler(interface_t interface, volatile bot_state_t* state,
                String argument) {
  // set mode to uci, this is the only interface supported so we do nothing

  fprintf(interface.output_stream, "id\n");

  fprintf(interface.output_stream, "option\n");

  fprintf(interface.output_stream, "uciok\n");

  return 0;
}

handler_for(debug) {
  if (string_compare(argument, string_literal("on"))) {
    state->debug_mode = 1;
    return 0;
  }
  if (string_compare(argument, string_literal("off"))) {
    state->debug_mode = 0;
    return 0;
  }
  return 1;
}

handler_for(isready) {
  while (!state->is_ready);  // prolly a better way to do this
  if (state->is_ready) {
    fprintf(interface.output_stream, "readyok\n");
    return 0;
  }
}

handler_for(setoption) {
  size_t index = 0;
  enum {
    SETOPTION_START,
    SETOPTION_GET_NAME,
    SETOPTION_GET_VALUE,
  };
  int state = SETOPTION_START;
  size_t option_name_start, option_name_end, option_value_start,
      option_value_end;
  String remainder = argument;
  while (remainder.length > 0) {
    size_t token_end = next_char(remainder, ' ');
    String token = substring(remainder, 0, token_end);
    remainder = substring(remainder, token_end + 1, remainder.length);

    if (state == SETOPTION_START) {
      if (string_compare(token, string_literal("name"))) {
        state = SETOPTION_GET_NAME;
        option_name_start = token_end + 1;
        continue;
      }
      return 1;
    }
    if (state == SETOPTION_GET_NAME) {
      if (string_compare(token, string_literal("value"))) {
        state = SETOPTION_GET_VALUE;
        option_value_start = token_end + 1;
        continue;
      }
      option_name_end = token_end;
    }
    if (state == SETOPTION_GET_VALUE) {
      option_value_end = token_end;
    }
  }
  String option_name = substring(argument, option_name_start, option_name_end);
  String option_value =
      substring(argument, option_value_start, option_value_end);

  // do studf with the option name and option value.
  return 1;
}

handler_for(register) {
  size_t index = 0;
  if (string_compare(argument, string_literal("later"))) {
    return 0;
  }
  enum {
    REGISTER_START,
    REGISTER_GET_NAME,
    REGISTER_GET_CODE,
  };
  int state = REGISTER_START;
  size_t name_start, name_end, code_start, code_end;
  String remainder = argument;
  while (remainder.length > 0) {
    size_t token_end = next_char(remainder, ' ');
    String token = substring(remainder, 0, token_end);
    remainder = substring(remainder, token_end + 1, remainder.length);
    if (state == REGISTER_START) {
      if (string_compare(token, string_literal("name"))) {
        state = REGISTER_GET_NAME;
        name_start = token_end + 1;
        continue;
      }
      if (string_compare(token, string_literal("code"))) {
        state = REGISTER_GET_CODE;
        code_start = token_end + 1;
        continue;
      }
      return 1;
    }
    if (state == REGISTER_GET_NAME) {
      if (string_compare(token, string_literal("code"))) {
        state = REGISTER_GET_CODE;
        code_start = token_end + 1;
        continue;
      }
      name_end = token_end;
    }
    if (state == REGISTER_GET_CODE) {
      if (string_compare(token, string_literal("name"))) {
        state = REGISTER_GET_NAME;
        name_start = token_end + 1;
        continue;
      }
      code_end = token_end;
    }
  }
  String name = substring(argument, name_start, name_end);
  String code = substring(argument, code_start, code_end);

  // do stuff with the name and code.
  return 1;
}

handler_for(ucinewgame) { assert(0 && "TODO: make handler for ucinewgame"); }

handler_for(position) { assert(0 && "TODO: make handler for position"); }

handler_for(go) { assert(0 && "TODO: make handler for go"); }

handler_for(stop) { assert(0 && "TODO: make handler for stop"); }

handler_for(ponderhit) { assert(0 && "TODO: make handler for ponderhit"); }

handler_for(quit) { assert(0 && "TODO: make handler for quit"); }

int add_handler(String cmd, handler_t* handler) {
  size_t hash = string_hash(cmd);
  size_t index = hash % HANDLER_BUCKET_COUNT;
  handler_key_value_t handler = handlers[index];
  if (handlers[index].hash != 0) {  // hash collision
    return 1;
  }
  handlers[index] =
      (handler_key_value_t){.hash = hash, .cmd = cmd, .fn = handler};
}

handler_t uci_handler;

int build_handler_table() {
  int hash_collision_count = 0;
  hash_collision_count += add_handler(string_literal("uci"), uci_handler);
  // more insertions
  return hash_collision_count;  // build is only successful if there are no
                                // hash_collisions
}
