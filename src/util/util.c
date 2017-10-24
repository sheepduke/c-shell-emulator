#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <unistd.h>

// ======================================================================
// Internal
// ======================================================================
#ifndef PRINT_LOG
#define PRINT_LOG(...) printf(__VA_ARGS__)
#endif

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

void info(const char *format, ...) {
  printf("[info] ");
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
  printf("[ERROR] ");
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("\n");  
}
