#include "str.h"

#include <assert.h>
#include <string.h>

const int STRING_SIZE_CHUNK = 1024;


// ======================================================================
//  Interval Functions
// ======================================================================

struct String {
  size_t length;
  size_t capacity;
  char *data;
};


// Increase the capacity of string to safely hold `target'.
// 
static void string_grow_capacity(String *string, size_t length) {
  while (string->capacity < length + 1) {
	string->capacity += STRING_SIZE_CHUNK;
	void *new_data = realloc(string->data, string->capacity);
	assert(new_data);
	if (!new_data) {
	  // empty
	}
  }
}

static bool is_space(char ch) {
  return (ch == '\r' || ch == '\n' || ch == ' ');
}

// ======================================================================
//  API
// ======================================================================

String *string_new() {
  String *string = malloc(sizeof(String));

  // string_clear will initialize it.
  string->data = NULL;
  string_clear(string);
  
  return string;
}

String *string_from(const char *data) {
  String *string = string_new();

  string_append(string, data);

  return string;
}

void string_copy(String *str, const String *source) {
  assert(str && source && source->data);

  string_set(str, string_raw(source));
}

void string_clear(String *string) {
  free(string->data);
  string->data = malloc(sizeof(char) * STRING_SIZE_CHUNK);
  string->data[0] = '\0';
  string->length = 0;
  string->capacity = STRING_SIZE_CHUNK;
}

void string_destroy(void *str) {
  String *str_ = str;
  if (str_) {
	free(str_->data);
	free(str_);
  }
}

bool string_empty(const String *str) {
  return (string_length(str) == 0);
}

bool string_equal(const String *str1, const String *str2) {
  assert(str1 && str2);
  return (strcmp(str1->data, str2->data) == 0);
}

size_t string_length(const String *string) {
  return string->length;
}

char *string_raw(const String *string) {
  return string->data;
}

char string_at(const String *str, size_t index) {
  assert(str && str->data);
  assert(index < str->length);

  return str->data[index];
}

char string_last(const String *str) {
  assert(str && str->data);

  return str->data[str->length - 1];
}

size_t string_find(const String *str, char ch) {
  for (size_t i = 0; i < str->length; i++) {
	if (string_at(str, i) == ch) {
	  return i;
	}
  }

  return -1;
}

void string_set(String *str, const char *content) {
  string_clear(str);
  string_append(str, content);
}

void string_push(String *string, char ch) {
  assert(string);

  size_t new_length = string->length + 1;
  string_grow_capacity(string, new_length);
  string->data[string->length] = ch;
  string->data[new_length] = '\0';
  string->length++;
}

void string_append(String *string, const char *data) {
  assert(string);

  if (data && strlen(data) > 0) {
	size_t new_length = string->length + strlen(data);
	string_grow_capacity(string, new_length);
	string->length = new_length;
	strcat(string->data, data);
  }
}

void string_concat(String *str, const String *source) {
  assert(str && source);
  string_append(str, source->data);
}

void string_sub(String *str, size_t start, size_t end) {
  assert(start < string_length(str) + 1 && end < string_length(str) + 1);

  size_t index = 0;

  for (size_t i = start; i < end; i++, index++) {
	str->data[index] = str->data[i];
  }
  str->data[index] = '\0';
  str->length = end - start;
}

void string_trim(String *str) {
  size_t start = string_length(str);
  
  for (size_t i = 0; i < string_length(str); i++) {
	if (!is_space(str->data[i])) {
	  start = i;
	  break;
	}
  }

  size_t end = string_length(str);
  for (size_t i = string_length(str) - 1; i >= start; i--) {
	if (!is_space(str->data[i])) {
	  end = i + 1;
	  break;
	}
  }

  string_sub(str, start, end);
}
