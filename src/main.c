#include <stdio.h>
#include <assert.h>

#include "shell.h"

#include "vector.h"
#include <string.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// void exec(char *name, int fd_in[2], int fd_out[2]) {
//   close(fd_in[0]);
//   close(fd_out[1]);

//   dup2(fd_in[1], STDIN_FILENO);
//   dup2(STDOUT_FILENO, fd_out[0]);

//   char **buf = malloc(1024);
//   buf[0] = name;
//   execvp(name, buf);

//   close(fd_out[0]);
//   close(fd_in[0]);
// }

// int main(int argc, char *argv[]) {
//   int fd_in[2];
//   int fd_out[2];

//   pid_t child_pid;

//   if (pipe(fd_in) == -1 || pipe(fd_out) == -1) {
// 	perror("pipe");
// 	return 1;
//   }

//   child_pid = fork();
//   if (child_pid == -1) {
// 	perror("fork");
// 	return 1;
//   }

//   close(fd_in[0]);
//   dup2(STDIN_FILENO, fd_in[1]);


//   if (child_pid == 0) {
// 	exec(argv[1], fd_in, fd_out);

// 	return 0;
//   }
//   else {            /* Parent writes argv[1] to pipe */
// 	close(fd_out[1]);
// 	dup2(fd_out[0], STDOUT_FILENO);
// 	close(fd_in[0]);
// 	close(fd_out[0]);
// 	wait(NULL);                /* Wait for child */
// 	return 0;
//   }
// }

#include "vector.h"
#include "util.h"
#include <errno.h>
#include "command.h"

void exec(command *cmd, int fd_in[2], int fd_out[2]) {
  close(fd_in[1]);
  close(fd_out[0]);

  dup2(fd_in[0], STDIN_FILENO);
  dup2(fd_out[1], STDOUT_FILENO);

  char **buf = malloc(1024);
  buf[0] = string_raw(command_name(cmd));
  execvp(buf[0], buf);

  close(fd_in[0]);
  close(fd_out[1]);
}

void spawn_command(vector *commands, vector *pipe_in, vector *pipe_out, int index) {
  if (index < vector_size(commands)) {
	pid_t child_pid = fork();
	if (child_pid == -1) {
	  fatal_error("Fork failed: %s", strerror(errno));
	}
	else if (child_pid == 0) {
	  // Child process
	  if (index < vector_size(commands)) {
		spawn_command(commands, pipe_in, pipe_out, index + 1);
		printf("Child %d\n", index);
	  }
	  else {
		return ;
	  }
	}
  }
}

// int main() {
//   vector *commands = vector_new();

//   vector_push(commands, command_new(string_from("ls")));
//   vector_push(commands, command_new(string_from("ls")));

//   spawn_command(commands, NULL, NULL, 0);

//   for (int i = 0; i < vector_size(commands); i++) {
// 	command_destroy(vector_at(commands, i));
//   }
 

//   return 0;
// }

int main() {
  vector *commands = vector_new();
  vector *pipes = vector_new();

  // TEST
  vector_push(commands, command_new(string_from("ls")));
  vector_push(commands, command_new(string_from("cat")));
  vector_push(commands, command_new(string_from("cat")));
  
  size_t command_count = vector_size(commands);
  
  for (int i = 0; i <= command_count; i++) {
	int *pipe_fd = malloc(sizeof(int) * 2);
	pipe(pipe_fd);

	if (i == 0) {
	  close(pipe_fd[1]);
	  dup2(STDIN_FILENO, pipe_fd[0]);
	}
	else if (i == command_count) {
	  close(pipe_fd[0]);
	  dup2(STDOUT_FILENO, pipe_fd[1]);
	}

	vector_push(pipes, pipe_fd);
  }

  for (int i = 0; i < command_count; i++) {
	pid_t pid = fork();

	
	int *pipe_in = vector_at(pipes, i);
	int *pipe_out = vector_at(pipes, i+1);

	if (pid == 0) {
	  exec(vector_at(commands, i), pipe_in, pipe_out);
	  return 0;
	}
	else {

	}
  }
  wait(NULL);
  printf("All Finished.\n");
}
