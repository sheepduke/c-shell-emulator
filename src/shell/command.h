#ifndef COMMAND_H
#define COMMAND_H

#include "str.h"
#include "vector.h"

typedef struct Command Command;

Command *command_new(String *name);

void command_destroy(void *cmd);

String *command_name(const Command *cmd);

void command_to_string(const Command *cmd, String *str);

void command_exec(Command *cmd, int fd_in[2], int fd_out[2]);

void execute_command_sequence(Vector *commands);
  
#endif
