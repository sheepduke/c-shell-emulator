#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <unistd.h>

// ======================================================================
// API
// ======================================================================


int read_line(String *str, FILE *stream) {
  assert(str);
  
  size_t size = 0;
  char *buffer = NULL;

  int result = getline(&buffer, &size, stream);

  if (result != -1) {
	string_append(str, buffer);
  }

  return result;
}

void debug(const char *format, ...) {
#ifndef NDEBUG
  printf("[debug] ");
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("\n");
#endif
}

void info(const char *format, ...) {
#ifndef NDEBUG
  printf("[info] ");
#endif
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("\n");
}

void warn(const char *format, ...) {
  printf("[Warning] ");
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("\n");
}

void error(const char *format, ...) {
  fprintf(stderr, "[ERROR] ");
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("\n");  
}

void fatal_error(const char *format, ...) {
  fprintf(stderr, "FATAL ERROR: ");
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("\n");
  exit(1);
}
