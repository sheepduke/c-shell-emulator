#include "command.h"
#include "vector.h"
#include "str.h"
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
  Vector *io;
};


struct CommandIO {
  CommandIODirection direction;
  const char *filename;
  int old;
  int new;
};

bool pid_equal(const void *element, const void *value) {
  return *(int *)element == *(int *)value;
}


// ======================================================================
// API
// ======================================================================

Command *command_new(String *name) {
  Command *command = malloc(sizeof(Command));

  command->name = name;
  command->args = vector_new(string_destroy);

  return command;  
}


void command_destroy(void *cmd) {
  Command *cmd_ = cmd;
  string_destroy(cmd_->name);
  vector_destroy(cmd_->args);
  free(cmd);
}

const String *command_name(const Command *cmd) {
  return cmd->name;
}

const Vector *command_args(const Command *command) {
  return command->args;
}

void command_to_string(const Command *cmd, String *str) {
  string_append(str, "Command: ");
  string_concat(str, cmd->name);
  for (int i = 0; i < vector_size(cmd->args); i++) {
	string_append(str, " ");
	string_concat(str, vector_at(cmd->args, i));
  }
}

void command_push_arg(Command *command, String *arg) {
  vector_push(command->args, arg);
}

void command_exec(const Command *command, int fd_in[2], int fd_out[2]) {
  // Handle I/O redirection.
  close(fd_in[1]);
  close(fd_out[0]);

  dup2(fd_in[0], STDIN_FILENO);
  dup2(fd_out[1], STDOUT_FILENO);

  // Prepare command buffers.
  const Vector *args = command_args(command);
  char **buffer = malloc(sizeof(char *) * (vector_size(args) + 1));
  buffer[0] = string_raw(command_name(command));

  for (int i = 1; i <= vector_size(args); i++) {
    buffer[i] = string_raw(vector_at(args, i-1));
  }

  execvp(buffer[0], buffer);

  close(fd_in[0]);
  close(fd_out[1]);
}

void execute_command_sequence(Vector *commands) {
  for (int i = 0; i < vector_size(commands); i++) {
    execute_command_and(vector_at(commands, i));
  }
}

void execute_command_and(Vector *commands) {
  for (int i = 0; i < vector_size(commands); i++) {
    execute_command_pipe(vector_at(commands, i));
  }
}

void execute_command_pipe(Vector *commands) {

#ifndef NDEBUG
  info("Executing command pipe:");
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

  Vector *pid_list = vector_new(free);
  
  // Prepare pipes.
  for (int i = 0; i <= command_count; i++) {
	int *pipe_fd = malloc(sizeof(int) * 2);
    if (pipe(pipe_fd) == -1) {
      error("System call error: cannot create pipe.");
      exit(1);
    }

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

  // Fork processes.
  for (int i = 0; i < command_count; i++) {
    pid_t pid = fork();

    if (pid == -1) {
      fatal_error("Fatal error: System call error: cannot fork.");
    }
    else if (pid != 0) {
      debug("Forked PID: %d", pid);
      int *pid_p = malloc(sizeof(int));
      *pid_p = pid;
      vector_push(pid_list, pid_p);
    }
    else if (pid == 0) {
      int *pipe_in = vector_at(pipes, i);
      int *pipe_out = vector_at(pipes, i+1);

      command_exec(vector_at(commands, i), pipe_in, pipe_out);
      return ;
    }
  }

  waitpid(*(int *)vector_at(pid_list, 0), NULL, 0);
  waitpid(*(int *)vector_at(pid_list, vector_size(pid_list)-1), NULL, 0);
  
  vector_destroy(pid_list);
  vector_destroy(pipes);
}
