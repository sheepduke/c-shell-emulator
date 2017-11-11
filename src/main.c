#include <stdio.h>
#include <assert.h>

#include "shell.h"


#include "vector.h"
#include "util.h"
#include <errno.h>
#include "command.h"
#include "cmd_parser.h"

// int main(int argc, char *argv[]) {
//   Vector *commands = vector_new(command_destroy);

//   for (int i = 1; i < argc; i++) {
// 	vector_push(commands, command_new(string_from(argv[i])));
//   }

//   execute_command_sequence(commands);

//   vector_destroy(commands);
// }


// int main() {

//   shell_start();

//   return 0;
// }

int main() {

  Vector *commands = vector_new(command_destroy);

  Command *command = command_new(string_from("echo"));
  command_push_arg(command, string_from("asdf"));
  vector_push(commands, command);

  command = command_new(string_from("cat"));
  vector_push(commands, command);

  command =command_new(string_from("sleep"));
  command_push_arg(command, string_from("3"));

  vector_push(commands, command);
  
  execute_command_pipe(commands);

  return 0;
}
