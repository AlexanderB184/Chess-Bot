#include "String.h"

#include "Slice.h"

String substring(String string, size_t start, size_t end) {
  return (String){string.buffer + start, end - start};
}
String string_clone(Arena* arena, String string) {
  size_t length = string.length;
  return (String){memcpy(arena_push((arena), length), string.buffer, length),
                  string.length};
}
String string_concat(Arena* arena, String lslice, String rslice) {
  size_t left_length = lslice.length;
  size_t right_length = rslice.length;
  return (String){memcpy(memcpy(arena_push((arena), left_length + right_length),
                                lslice.buffer, left_length) +
                             left_length,
                         rslice.buffer, right_length),
                  lslice.length + rslice.length};
}

bool string_compare(String lslice, String rslice) {
  return lslice.length == rslice.length &&
         memcmp(lslice.buffer, rslice.buffer, lslice.length);
}

Slice(String) split_string(Arena* arena, String string, String delim) {
  size_t end = 0;
  // init slice with 1 thing
  Slice(String) slice = {arena_push(arena, sizeof(string)), 1};
  // while we have not traversed the entire string
  while (end < string.length) {
    // expand list to store next item
    arena_push(arena, sizeof(string));
    // move to next delim
    end = find_chars(string, delim);
    // get substring and add to list
    slice.buffer[slice.length - 1] = substring(string, 0, end);
    // move string along skipping the delim
    string = substring(string, end + 1, string.length);
  }
  arena_pop(arena, sizeof(string));  // we over allocate by 1 so we truncate to
                                     // get back to where we should be
  return slice;
}

String find_substring(String haystack, String needle) {
  for (size_t i = 0; i < haystack.length - needle.length; i++) {
    String guess = substring(haystack, i, i + needle.length);
    if (string_compare(guess, needle)) {
      return guess;
    }
  }
  return (String){0};
}

size_t find_char(String string, char delim) {
  size_t i;
  for (i = 0; i < string.length; i++) {
    if (string.buffer[i] == delim) {
      return i;
    }
  }
  return i;
}

size_t find_chars(String string, String delim) {
  size_t i;
  for (i = 0; i < string.length; i++) {
    if (find_char(delim, string.buffer[i]) != delim.length) {
      return i;
    }
  }
  return i;
}

String string_reverse(String string) {
  size_t half_length = string.length / 2;
  for (size_t i = 0; i < half_length; i++) {
    char t = string.buffer[i];
    string.buffer[i] = string.buffer[string.length - 1 - i];
    string.buffer[string.length - 1 - i] = t;
  }
  return string;
}

String white_space = string_literal(" \t\r\n");

String lstrip_string(String string) {
  size_t i;
  for (i = 0; i < string.length; i++) {
    if (find_char(white_space, string.buffer[i]) == white_space.length) {
      break;
    }
  }
  return substring(string, i, string.length);
}

String rstrip_string(String string) {
  size_t i;
  for (i = string.length - 1; i > 0; i--) {
    if (find_char(white_space, string.buffer[i]) == white_space.length) {
      break;
    }
  }
  return substring(string, 0, i+1);
}

String strip_string(String string) {
  return lstrip_string(rstrip_string(string));
}

char* as_cstr(Arena* arena, String string) {
  char* cstr = arena_push_array(arena, char, string.length + 1);
  memcpy(cstr, string.buffer, string.length);
  cstr[string.length] = '\0';
  return cstr;
}

bool string_endswith(String string, String end) {
  return string_compare(
      substring(string, string.length - end.length, string.length), end);
}
bool string_startswith(String string, String start) {
  return string_compare(substring(string, 0, start.length), start);
}

int string_to_int(String number) {
  size_t i = 0;
  int num = 0;
  while (number.buffer[i] >= '0' && number.buffer[i] <= '9')
    num = num * 10 + number.buffer[i++] - '0';
  return num;
}
float string_to_float(String number) { return 0.1; }
String int_to_string(Arena* arena, int number) {
  if (number == 0) {
    String str = {.buffer = arena->memory + arena->position, .length = 1};
    *arena_push_struct(arena, char) = '0';
    return str;
  }
  String str = {.buffer = arena->memory + arena->position, .length = 0};
  while (number) {
    *arena_push_struct(arena, char) = number % 10 + '0';
    str.length++;
    number /= 10;
  }
  string_reverse(str);
  return str;
}
String float_to_string(Arena* arena, float number) { return (String){}; }