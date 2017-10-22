#ifndef COMMAND_H
#define COMMAND_H

#include "str.h"

typedef struct command command;

command *command_new(string *name);

void command_destroy(command *cmd);

string *command_name(command *cmd);

void command_exec(command *cmd, int fd_in[2], int fd_out[2]);

#endif
