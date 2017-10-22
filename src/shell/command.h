#ifndef COMMAND_H
#define COMMAND_H

#include "str.h"
#include "vector.h"

typedef struct command command;

command *command_new(string *name);

void command_destroy(void *cmd);

string *command_name(const command *cmd);

void command_to_string(const command *cmd, string *str);

void command_exec(command *cmd, int fd_in[2], int fd_out[2]);

void execute_command_sequence(vector *commands);
  
#endif
