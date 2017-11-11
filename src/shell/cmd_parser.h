#ifndef CMD_PARSER_H
#define CMD_PARSER_H

#include <stdlib.h>
#include <stdbool.h>

#include "str.h"
#include "vector.h"

bool parse(const String *input, Vector *commands);

#endif
