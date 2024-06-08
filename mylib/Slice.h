#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "Arena.h"

// generic slice
// all custom typed slices must be implemented with this interface to correctly
// work with the slice macros:
// struct Slice {
//  type* buffer;
//  size_t length;
// };
typedef struct Slice {
  void* buffer;
  size_t length;
} Slice;

#define push_slice(type, arena, slice, item) \
  *arena_push_struct(arena, type) = item;    \
  (slice).length++;

#define make_slice(type, arena) \
  (Slice(type)) { .buffer = (arena)->memory + (arena)->position, .length = 0 }

#define Slice(type) type##_Slice

#define sub_slice(type, slice, start, end) \
  type##_sub_slice((slice), (start), (end))

#define clone_slice(type, arena, slice) type##_clone_slice((arena), (slice))

#define concat_slice(type, arena, lslice, rslice) \
  type##_concat_slice((arena), (lslice), (rslice))

#define compare_slice(type, lslice, rslice) \
  type##_compare_slice((lslice), (rslice))

#define Slice_proto(type)                                                 \
  typedef struct Slice(type) {                                            \
    type* buffer;                                                         \
    size_t length;                                                        \
  }                                                                       \
  Slice(type);                                                            \
  Slice(type)                                                             \
      type##_slice_subslice(Slice(type) slice, size_t start, size_t end); \
  Slice(type) type##_slice_clone(Arena* arena, Slice(type) slice);        \
  Slice(type) type##_slice_concat(Arena* arena, Slice(type) lslice,       \
                                  Slice(type) rslice);                    \
  bool type##_slice_compare(Slice(type) lslice, Slice(type) rslice);

#define Slice_impl(type)                                                       \
  Slice(type) type##_sub_slice(Slice(type) slice, size_t start, size_t end) {  \
    return (Slice(type)){slice.buffer + start, end - start};                   \
  }                                                                            \
  Slice(type) type##_clone_slice(Arena* arena, Slice(type) slice) {            \
    size_t length = slice.length * sizeof(type);                               \
    return (Slice(type)){                                                      \
        memcpy(arena_push((arena), length), slice.buffer, length),             \
        slice.length};                                                         \
  }                                                                            \
  Slice(type) type##_concat_slice(Arena* arena, Slice(type) lslice,            \
                                  Slice(type) rslice) {                        \
    size_t left_length = lslice.length * sizeof(type);                         \
    size_t right_length = rslice.length * sizeof(type);                        \
    return (Slice(type)){                                                      \
        memcpy(memcpy(arena_push((arena), left_length + right_length),         \
                      lslice.buffer, left_length) +                            \
                   left_length,                                                \
               rslice.buffer, right_length),                                   \
        lslice.length + rslice.length};                                        \
  }                                                                            \
  bool type##_slice_compare(Slice(type) lslice, Slice(type) rslice) {          \
    return lslice.length == rslice.length &&                                   \
           memcmp(lslice.buffer, rslice.buffer, lslice.length * sizeof(type)); \
  }

#define slice_literal(type, literal) \
  Slice(type) { (literal), sizeof(literal) / sizeof(type) }

/*
#define slice_clone(arena, slice)                                         \
  {                                                                       \
    memcpy(                                                               \
      arena_push((arena), slice_byte_count(slice)),                       \
      (slice).buffer,                                                     \
      slice_byte_count(slice)                                             \
    ),                                                                    \
    (slice).length                                                        \
  }

#define slice_concat(arena, lslice, rslice) \
  { \
    memcpy( \
      memcpy( \
        arena_push((arena), slice_byte_count_combined(lslice, rslice)), \
        (lslice).buffer, \
        slice_byte_count(lslice) \
      ) + slice_byte_count(lslice), \
      (rslice).buffer, \
      slice_byte_count(rslice) \
    ), \
    (lslice).length + (rslice).length \
  }

#define slice_subslice(slice, start, end) \
  { (slice).buffer + (start), (end) - (start) }

#define slice_compare(lslice, rslice) (                                   \
    (lslice).length == (rslice).length                                    \
  && memcmp((lslice).buffer, (rslice).buffer, slice_byte_count(lslice))   \
  )




#define slice_byte_count(slice) ((slice).length * sizeof(*(slice).buffer))
#define slice_byte_count_combined(slice1, slice2) \
  (((slice1).length + (slice2).length) * sizeof(*(slice).buffer))
*/