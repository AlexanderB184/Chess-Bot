#pragma once

#include <stddef.h>

// linear allocator
// allocates by push memory to the top
// can only release memory by popping from the top or clearing
typedef struct Arena {
  void* memory;
  size_t position;
} Arena;

// pushes nbytes onto the arena
void* arena_push(Arena* arena, size_t nbytes);
// pushes nbytes onto the arena and sets them to 0
void* arena_push_zero(Arena* arena, size_t nbytes);
// pushes a struct onto the arena
#define arena_push_struct(arena, type) (type*)arena_push((arena), sizeof(type))
// pushes a struct onto the arena and initializes all values to 0
#define arena_push_struct_zero(arena, type) \
  (type*)arena_push_zero((arena), sizeof(type))
// pushes an array onto the arena
#define arena_push_array(arena, type, nmembs) \
  (type*)arena_push((arena), sizeof(type) * (nmembs))
// pushes an array onto the arena and initializes all values to 0
#define arena_push_array_zero(arena, type, nmembs) \
  (type*)arena_push_zero((arena), sizeof(type) * (nmembs))
// pops nbytes from the arena
void arena_pop(Arena* arena, size_t nBytes);
// clears the arena
void arena_clear(Arena* arena);
// gets the current position, can be used with set pos to restore the arena to a
// previous state
size_t arena_get_pos(Arena* arena);
// sets the position to a specified value, can be used with get pos to restore
// the arena to a previous state
void arena_set_pos(Arena* arena, size_t);

// allocates a new arena
Arena* arena_alloc();
// releases an arena
void arena_release(Arena*);

typedef struct Scratch_Arena {
  Arena* arena;
  size_t previous_state;
} Scratch_Arena;

// finds an arena which is not in conflicts, if none exist it allocates a new
Scratch_Arena get_scratch(Arena** conflicts, size_t conflict_count);

// restores scratch arena to previous state
void release_scratch(Scratch_Arena scratch);