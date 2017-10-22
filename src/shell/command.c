#include "command.h"
#include "vector.h"

#include <stdlib.h>
#include <unistd.h>

// ======================================================================
// Internal
// ======================================================================

struct command {
  string *name;
  vector *args;
};

// ======================================================================
// API
// ======================================================================

command *command_new(string *name) {
  command *cmd = malloc(sizeof(command));

  cmd->name = string_new();
  string_copy(cmd->name, name);

  cmd->args = vector_new();

  return cmd;
}


void command_destroy(command *cmd) {
  string_destroy(&cmd->name);
  vector_destroy(&cmd->args);
  free(cmd);
}

string *command_name(command *cmd) {
  return cmd->name;
}

void command_exec(command *cmd, int fd_in[2], int fd_out[2]) {
  // int args_size = vector_size(cmd->args);

  // char **args = malloc(args_size * sizeof(char *));

  // dup2(fd_in, )

  // execvp(string_raw(cmd->name));
}
