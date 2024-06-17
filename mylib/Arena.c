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
      arenas[i] = (Arena){malloc(0x10000), 0};
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

#include <stdio.h>

// finds an arena which is not in conflicts, if none exist it allocates a new
Scratch_Arena get_scratch(Arena** conflicts, size_t conflict_count) {
  /*Arena* free_arena = 0;
  size_t emptiest_position = 0x1000000;
  Arena* emptiest = 0;
  for (size_t i = 0; i < MAX_ARENAS; i++) {
    if (!free_arena && arenas[i].memory == NULL) {
      free_arena = &arenas[i];
    }
    int has_conflict = 0;
    for (size_t j = 0; j < conflict_count; j++) {
      if (&arenas[i] == conflicts[j]) {
        has_conflict = 1;
      }
    }
    if (!has_conflict && arenas[i].position < emptiest_position) {
      emptiest_position = arenas[i].position;
      emptiest = &arenas[i];
    }
  }
  if (free_arena) {
    *free_arena = (Arena){malloc(0x10000), 0};
    return (Scratch_Arena){free_arena, 0};
  } else {
    return (Scratch_Arena){emptiest, emptiest_position};
  }*/
  static Arena arena = {NULL,0};
  if (arena.memory == NULL) {
    arena.memory = malloc(10 * 1000 * 1000);
  }
  //printf("%zu\n",arena.position);
  return (Scratch_Arena){&arena, arena.position};
}

// restores scratch arena to previous state
void release_scratch(Scratch_Arena scratch) {
  scratch.arena->position = scratch.previous_state;
  //if (scratch.previous_state == 0) {
  //  arena_release(scratch.arena);
  //}
}