#include <stdio.h>
#include <assert.h>

#include "shell.h"


#include "vector.h"
#include "util.h"
#include <errno.h>
#include "command.h"



int main(int argc, char *argv[]) {
  vector *commands = vector_new(command_destroy);

  for (int i = 1; i < argc; i++) {
	vector_push(commands, command_new(string_from(argv[i])));
  }

  
  info("Executing command sequence:");
  for (int i = 0; i < vector_size(commands); i++) {
	string *cmd_string = string_new();
	const command *cmd = vector_at(commands, i);
	command_to_string(cmd, cmd_string);
	printf(" | %s\n", string_raw(cmd_string));
	string_destroy(cmd_string);
  }


  execute_command_sequence(commands);

  vector_destroy_all(commands);
}
