#include "command.h"
#include "vector.h"
#include "util.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// ======================================================================
// Internal
// ======================================================================

struct Command {
  String *name;
  Vector *args;
};

// ======================================================================
// API
// ======================================================================

Command *command_new(String *name) {
  Command *cmd = malloc(sizeof(Command));

  cmd->name = name;

  cmd->args = vector_new(string_destroy);

  return cmd;
}


void command_destroy(void *cmd) {
  Command *cmd_ = cmd;
  string_destroy(cmd_->name);
  vector_destroy(cmd_->args);
  free(cmd);
}

String *command_name(const Command *cmd) {
  return cmd->name;
}

void command_to_string(const Command *cmd, String *str) {
  string_append(str, "Command: ");
  string_concat(str, cmd->name);
  for (int i = 0; i < vector_size(cmd->args); i++) {
	string_concat(str, vector_at(cmd->args, i));
	string_append(str, " ");
  }
}

void command_exec(Command *cmd, int fd_in[2], int fd_out[2]) {
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

void execute_command_sequence(Vector *commands) {

#ifndef NDEBUG
  info("Executing command sequence:");
  String *cmd_string = string_new();
  for (int i = 0; i < vector_size(commands); i++) {
	Command *cmd = vector_at(commands, i);
	command_to_string(cmd, cmd_string);
	info(" | %s", string_raw(cmd_string));
	string_clear(cmd_string);
  }
  string_destroy(cmd_string);
#endif
  
  Vector *pipes = vector_new(free);
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
	  command_exec(vector_at(commands, i), pipe_in, pipe_out);
	  return ;
	}
	else {

	}
  }
  wait(NULL);

  vector_destroy_all(pipes);
}
