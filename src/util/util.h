#ifndef UTIL_H
#define UTIL_H

#include "str.h"

#include <stdio.h>

int read_line(String *str, FILE *stream);

void debug(const char *format, ...);

void info(const char *format, ...);

void warn(const char *format, ...);

void error(const char *format, ...);

void fatal_error(const char *format, ...);

#endif
