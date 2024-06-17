#pragma once

#include <string.h>

#include "Arena.h"
#include "Funcs.h"
#include "Slice.h"

// implementation of a simple vector type using arena allocators

#define Vector(type) type##_vector_t

#define vector_new(type, arena, vector, capacity) \
  type##_vector_new(arena, vector, capacity)

#define vector_push(type, arena, vector, item) \
  type##_vector_push(arena, vector, item)

#define vector_qpush(type, vector, item) type##_vector_qpush(vector, item)

#define vector_pop(type, vector) type##_vector_pop(vector)

#define vector_qop(type, vector) type##_vector_qop(vector)

#define vector_insert(type, arena, vector, idx, item) \
  type##_vector_insert(arena, vector, idx, item)

#define vector_qinsert(type, vector, idx, item) \
  type##_vector_qinsert(vector, idx, item)

#define vector_concat(type, arena, vector, rvector) \
  type##_vector_concat(arena, vector, rvector)

#define vector_qinsert(type, vector, rvector) \
  type##_vector_qconcat(vector, rvector)

#define vector_expand(type, arena, vector, size) \
  type##_vector_expand(arena, vector, size)

#define vector_clear(type, vector) type##_vector_clear(vector)

#define vector_as_slice(type, vector) \
  (Slice(type)) { vector.items, vector.count }

#define vector_proto(type)                                                \
                                                                          \
  typedef struct type##_vector_t type##_vector_t;                         \
  struct type##_vector_t {                                                \
    type* items;                                                          \
    size_t count, capacity;                                               \
  };                                                                      \
                                                                          \
  void type##_vector_new(Arena*, type##_vector_t*, size_t);               \
                                                                          \
  void type##_vector_push(Arena*, type##_vector_t*, type);                \
  void type##_vector_qpush(type##_vector_t*, type);                       \
                                                                          \
  void type##_vector_pop(type##_vector_t*);                               \
  void type##_vector_qpop(type##_vector_t*);                              \
                                                                          \
  void type##_vector_insert(Arena*, type##_vector_t*, size_t, type);      \
  void type##_vector_qinsert(Arena*, type##_vector_t*, size_t, type);     \
                                                                          \
  void type##_vector_concat(Arena*, type##_vector_t*, type##_vector_t*);  \
  void type##_vector_qconcat(Arena*, type##_vector_t*, type##_vector_t*); \
                                                                          \
  void type##_vector_expand(Arena*, type##_vector_t*, size_t);            \
  void type##_vector_clear(type##_vector_t*);

#define vector_impl(type)                                                      \
                                                                               \
  void type##_vector_new(Arena* arena, type##_vector_t* vector,                \
                         size_t capacity) {                                    \
    vector->items = arena_push_array(arena, type, capacity);                   \
    vector->count = 0;                                                         \
    vector->capacity = capacity;                                               \
  }                                                                            \
                                                                               \
  void type##_vector_push(Arena* arena, type##_vector_t* vector, type item) {  \
    if (vector->count >= vector->capacity) {                                   \
      type##_vector_expand(arena, vector,                                      \
                           vector->capacity + vector->capacity / 2);           \
    }                                                                          \
    type##_vector_qpush(vector, item);                                         \
  }                                                                            \
                                                                               \
  void type##_vector_qpush(type##_vector_t* vector, type item) {               \
    vector->items[vector->count++] = item;                                     \
  }                                                                            \
                                                                               \
  void type##_vector_pop(type##_vector_t* vector) {                            \
    if (vector->count > 0) vector->count--;                                    \
  }                                                                            \
                                                                               \
  void type##_vector_qpop(type##_vector_t* vector) { vector->count--; }        \
                                                                               \
  void type##_vector_insert(Arena* arena, type##_vector_t* vector, size_t idx, \
                            type item) {                                       \
    type##_vector_push(arena, vector, item);                                   \
    for (size_t i = vector->count - 1; i > idx; i++) {                         \
      type t = vector->items[i];                                               \
      vector->items[i] = vector->items[i - 1];                                 \
      vector->items[i - 1] = t;                                                \
    }                                                                          \
  }                                                                            \
  void type##_vector_qinsert(type##_vector_t* vector, size_t idx, type item) { \
    type##_vector_qpush(vector, item);                                         \
    for (size_t i = vector->count - 1; i > idx; i++) {                         \
      type t = vector->items[i];                                               \
      vector->items[i] = vector->items[i - 1];                                 \
      vector->items[i - 1] = t;                                                \
    }                                                                          \
  }                                                                            \
                                                                               \
  void type##_vector_concat(Arena* arena, type##_vector_t* vector,             \
                            type##_vector_t* rvector) {                        \
    if (vector->capacity < vector->count + rvector->count) {                   \
      type##_vector_expand(arena, vector, vector->count + rvector->count);     \
    }                                                                          \
    type##_vector_qconcat(vector, rvector);                                    \
  }                                                                            \
                                                                               \
  void type##_vector_qconcat(type##_vector_t* vector,                          \
                             type##_vector_t* rvector) {                       \
    memcpy(vector->items + vector->count, rvector->items, rvector->count);     \
    vector->count += rvector->count;                                           \
  }                                                                            \
                                                                               \
  void type##_vector_expand(Arena* arena, type##_vector_t* vector,             \
                            size_t size) {                                     \
    type* new_items = arena_push_array(arena, type, size);                     \
    vector->capacity = size;                                                   \
    vector->count = min(vector->count, vector->capacity);                      \
    vector->items = (type*)memcpy(new_items, vector->items, vector->count);    \
  }                                                                            \
                                                                               \
  void type##_vector_clear(type##_vector_t* vector) { vector->count = 0; }
