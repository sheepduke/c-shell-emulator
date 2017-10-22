#ifndef UTIL_H
#define UTIL_H

#include "str.h"

#include <stdio.h>

int read_line(string *str, FILE *stream);

void info(const char *format, ...);

void warn(const char *format, ...);

void error(const char *format, ...);

#endif
