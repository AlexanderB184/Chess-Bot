#include "uci.h"

typedef struct handler_key_value_t handler_key_value_t;

struct handler_key_value_t {
  size_t hash;
  String cmd;
  handler_t* fn;
};

#define HANDLER_BUCKET_COUNT 19

static handler_key_value_t handlers[HANDLER_BUCKET_COUNT] = {0};

int handle(interface_t interface, bot_state_t* state, Slice(String) args) {
  if (args.length < 1) {
    return 0;
  }
  String cmd = args.buffer[0];
  size_t hash = string_hash(cmd);
  size_t index = hash % HANDLER_BUCKET_COUNT;
  handler_key_value_t handler = handlers[index];
  if (hash == handler.hash && string_compare(cmd, handler.cmd)) {
    return handler.fn(interface, state, args);
  }
  return 0;
}

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

size_t string_hash(String str) { return 0ull; }