#include "str.h"
#include <assert.h>

#include <string.h>

const int STRING_SIZE_CHUNK = 1024;

struct string {
  size_t length;
  size_t capacity;
  char *data;
};

// ======================================================================
//  Interval Functions
// ======================================================================

// Increase the capacity of string to safely hold `target'.
// 
static void string_grow_capacity(string *string, size_t length) {
  while (string->capacity < length + 1) {
	string->capacity += STRING_SIZE_CHUNK;
	void *new_data = realloc(string->data, string->capacity);
	assert(new_data);
  }
}

// ======================================================================
//  API
// ======================================================================

string *string_new() {
  string *string = malloc(sizeof(string));

  string->data = NULL;
  string_clear(string);
  
  return string;
}

string *string_from(const char *data) {
  string *string = string_new();

  string_append(string, data);

  return string;
}

string *string_copy(string *source) {
  assert(source && source->data);

  return string_from(source->data);
}

void string_clear(string *string) {
  free(string->data);
  string->data = malloc(sizeof(char) * STRING_SIZE_CHUNK);
  string->data[0] = '\0';
  string->length = 0;
  string->capacity = STRING_SIZE_CHUNK;
}

void string_destroy(string *string) {
  if (string) {
	free(string->data);
	free(string);
  }
}

size_t string_length(string *string) {
  return string->length;
}

const char *string_raw(string *string) {
  return string->data;
}

void string_append(string *string, const char *data) {
  assert(string && data);

  size_t new_length = string->length + strlen(data);
  string_grow_capacity(string, new_length);
  string->length = new_length;
  strcat(string->data, data);
}

void string_concat(string *str, string *source) {
  assert(str && source);
  string_append(str, source->data);
}
