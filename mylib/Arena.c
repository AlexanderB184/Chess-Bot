#include "Arena.h"
#include <stdlib.h>
#include <string.h>

// pushes nbytes onto the arena
void* arena_push(Arena* arena, size_t nbytes) {
  void* ptr = arena->memory + arena->position;
  arena->position += nbytes;
  return ptr;
}
// pushes nbytes onto the arena and sets them to 0
void* arena_push_zero(Arena* arena, size_t nbytes) {
  void* ptr = arena->memory + arena->position;
  memset(ptr, 0, nbytes);
  arena->position += nbytes;
  return ptr;
}

// pops nbytes from the arena
void arena_pop(Arena* arena, size_t nBytes) { arena->position -= nBytes; }
// clears the arena
void arena_clear(Arena* arena) { arena->position = 0; }
// gets the current position, can be used with set pos to restore the arena to a
// previous state
size_t arena_get_pos(Arena* arena) { return arena->position; }
// sets the position to a specified value, can be used with get pos to restore
// the arena to a previous state
void arena_set_pos(Arena* arena, size_t position) {
  arena->position = position;
}

#define MAX_ARENAS 16

Arena arenas[MAX_ARENAS];

// allocates a new arena
Arena* arena_alloc() {
  for (size_t i = 0; i < MAX_ARENAS; i++) {
    if (arenas[i].memory == NULL) {
      arenas[i] = (Arena){malloc(0x1000), 0};
      return &arenas[i];
    }
  }
  return NULL;  // max number of arenas already allocated
}
// releases an arena
void arena_release(Arena* arena) {
  // swap with last
  void* ptr = arena->memory;
  *arena = (Arena){NULL, 0};
  free(ptr);
}

// finds an arena which is not in conflicts, if none exist it allocates a new
Scratch_Arena get_scratch(Arena** conflicts, size_t conflict_count) {
  Arena* free_arena = 0;
  for (size_t i = 0; i < MAX_ARENAS; i++) {
    if (!free_arena && arenas[i].memory == NULL) {
      free_arena = &arenas[i];
    }
    for (size_t j = 0; j < conflict_count; j++) {
      if (&arenas[i] != conflicts[j]) {
        return (Scratch_Arena){&arenas[i], arenas[i].position};
      }
    }
  }
  *free_arena = (Arena){malloc(0x1000), 0};
  return (Scratch_Arena){free_arena, 0};
}

// restores scratch arena to previous state
void release_scratch(Scratch_Arena scratch) {
  scratch.arena->position = scratch.previous_state;
  if (scratch.previous_state == 0) {
    arena_release(scratch.arena);
  }
}