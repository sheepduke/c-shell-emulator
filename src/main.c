#include <stdio.h>
#include <assert.h>

#include "shell.h"


#include "vector.h"
#include "util.h"
#include <errno.h>
#include "command.h"
#include "cmd_parser.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>


// int main() {

//   shell_start();

//   return 0;
// }

// ----------------------------------------------------------------------

// int main() {

//   signal(SIGQUIT, freeze_process);

//   signal(SIGINT, kill_process);


//   Vector *commands = vector_new(command_destroy);


//   Command *command = command_new(string_from("echo"));
//   command_push_arg(command, string_from("asdf"));
//   vector_push(commands, command);

//   command = command_new(string_from("cat"));
//   vector_push(commands, command);

//   command =command_new(string_from("sleep"));
//   command_push_arg(command, string_from("3"));
//   vector_push(commands, command);

//   execute_command_pipe(commands);
 
//   return 0;
// }


// ----------------------------------------------------------------------

int main() {

  shell_start();

}
