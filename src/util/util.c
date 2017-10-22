#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <unistd.h>

int read_line(string *str, FILE *stream) {
  assert(str);
  
  size_t size = 0;
  char *buffer = NULL;

  int result = getline(&buffer, &size, stream);

  if (result != -1) {
	string_append(str, buffer);
  }

  return result;
}

void info(const char *format, ...) {
#ifndef NDEBUG
  string *buffer = string_from("[info] ");
  string_append(buffer, format);
  string_append(buffer, "\n");

  va_list args;
  va_start(args, format);
  vprintf(string_raw(buffer), args);
  va_end(args);

  string_destroy(buffer);
#endif
}

void warn(const char *format, ...) {
#ifndef NDEBUG
  string *buffer = string_from("[Warning] ");
  string_append(buffer, format);
  string_append(buffer, "\n");

  va_list args;
  va_start(args, format);
  vfprintf(stderr, string_raw(buffer), args);
  va_end(args);

  string_destroy(buffer);
#endif
}

void error(const char *format, ...) {
  string *buffer = string_new();

#ifndef NDEBUG
  string_append(buffer, "[ERROR] ");
#endif

  string_append(buffer, format);
  string_append(buffer, "\n");

  va_list args;
  va_start(args, format);

  vfprintf(stderr, string_raw(buffer), args);

  va_end(args);

#ifndef NDEBUG
  string_destroy(buffer);
#endif
}

void fatal_error(const char *format, ...) {
  string *buffer = string_new();

  string_append(buffer, "Fatal Error: ");
  string_append(buffer, format);
  string_append(buffer, "\n");

  va_list args;
  va_start(args, format);

  vfprintf(stderr, string_raw(buffer), args);

  va_end(args);

  string_destroy(buffer);

  exit(1);
}
