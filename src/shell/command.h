#ifndef COMMAND_H
#define COMMAND_H

#include "str.h"
#include "vector.h"

typedef struct Command Command;

Command *command_new(String *name);

void command_destroy(void *cmd);

const String *command_name(const Command *cmd);

const Vector *command_args(const Command *command);

void command_to_string(const Command *cmd, String *str);

void command_push_arg(Command *command, String *arg);

void command_exec(const Command *cmd, int fd_in[2], int fd_out[2]);

void execute_command_pipe(Vector *commands);

void execute_command_sequence(Vector *commands);

void execute_command_and(Vector *commands);

typedef enum CommandIODirection {
  INPUT,
  OUTPUT
} CommandIODirection;

typedef struct CommandIO CommandIO;

#endif
