#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "Arena.h"
#include "Slice.h"
#include "string.h"


typedef struct String {
  char* buffer;
  size_t length;
} String;

#define string_literal(cstr) \
  (String) { (cstr), sizeof(cstr) - 1 }

#define from_cstr(cstr) \
  (String) { (cstr), strlen(cstr) }

char* as_cstr(Arena* arena, String string);

// clone string in arena
String string_clone(Arena* arena, String string);

// concatenates the two strings and returns the result
String string_concat(Arena* arena, String l_string, String r_string);

// compares two strings
bool string_compare(String l_string, String r_string);

bool string_endswith(String string, String end);
bool string_startswith(String string, String end);

// returns a substring of string
String substring(String string, size_t start, size_t end);

// finds the first substring in haystack equal to needle and returns a reference
// to that substring
String find_substring(String haystack, String needle);

// interface is not great, how to handle char not found???
// maybe better to return a substring

// returns the substring starting at the first char in string which is equal to
// delim
size_t find_char(String string, char delim);

// returns the substring starting at the first char in string which appears in
// delim
size_t find_chars(String string, String delim);

Slice_proto(String);

Slice(String) split_string(Arena* arena, String string, String delim);

String lstrip_string(String string);
String rstrip_string(String string);
String strip_string(String string);

int string_to_int(String number);
float string_to_float(String number);
String int_to_string(Arena* arena, int number);
String float_to_string(Arena* arena, float number);