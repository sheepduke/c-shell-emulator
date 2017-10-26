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

//   vector_destroy_all(commands);
// }

int main() {

  shell_start();

  return 0;
}
