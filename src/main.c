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

void exec(char *name, int fd_in[2], int fd_out[2]) {
  close(fd_in[0]);
  close(fd_out[1]);

  dup2(fd_in[1], STDIN_FILENO);
  dup2(STDOUT_FILENO, fd_out[0]);

  char **buf = malloc(1024);
  buf[0] = name;
  execvp(name, buf);

  close(fd_out[0]);
  close(fd_in[0]);
}

int main(int argc, char *argv[]) {
  int fd_in[2];
  int fd_out[2];

  pid_t child_pid;

  if (pipe(fd_in) == -1 || pipe(fd_out) == -1) {
	perror("pipe");
	return 1;
  }

  child_pid = fork();
  if (child_pid == -1) {
	perror("fork");
	return 1;
  }

  close(fd_in[0]);
  dup2(STDIN_FILENO, fd_in[1]);


  if (child_pid == 0) {
	exec(argv[1], fd_in, fd_out);

	return 0;
  }
  else {            /* Parent writes argv[1] to pipe */
	close(fd_out[1]);
	dup2(fd_out[0], STDOUT_FILENO);
	close(fd_in[0]);
	close(fd_out[0]);
	wait(NULL);                /* Wait for child */
	return 0;
  }
}
