#include "shell.h"
#include "util.h"
#include "list.h"
#include "cmd_parser.h"
#include "command.h"

#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

// ----------------------------------------------------------------------
//  Internal
// ----------------------------------------------------------------------


struct Shell {
  String *pwd;
  List *jobs;
  pid_t current_process;
  Vector *variable_name;
  Vector *variable_value;
};

static Shell *the_shell;

static Shell *shell_new() {
  Shell *shell = malloc(sizeof(Shell));

  // Set home directory.
  shell->pwd = string_from(getenv("HOME"));
  if (string_empty(shell->pwd)) {
	error("Cannot locate $HOME directory.");
    error("Setting '/' as current working directory.");
	string_set(shell->pwd, "/");
  }

  // Set current directory
  if (chdir(string_raw(shell->pwd)) == -1) {
	error(strerror(errno));
  }
  
  debug("Starting shell environment...");
  debug("Current working directory: %s", string_raw(shell->pwd));

  shell->jobs = list_new(free);
  shell->current_process = 0;

  the_shell = shell;

  return shell;
}

static void shell_destroy(Shell *shell) {
  free(shell);
}

void shell_cd(Shell *shell, String *dir) {
  if (chdir(string_raw(dir)) == -1) {
	error("cd: %s", strerror(errno));
  }
  else {
	char dir_name[1024];
	if (getcwd(dir_name, 1024) == NULL) {
	  error(strerror(errno));
	}
	string_set(shell->pwd, dir_name);
  }
}

void print_prompt(Shell *shell) {
  printf("%s $ ", string_raw(shell->pwd));
  fflush(stdout);
}

void freeze_process(int signal) {
  // FIXME This might not work.
  info("");
  debug("Current pid: %d", the_shell->current_process);
  if (the_shell->current_process > 0) {
    kill(the_shell->current_process, SIGTSTP);
  }
  print_prompt(the_shell);
}

void kill_process(int signal) {
  // FIXME This might not work.
  info("");
  if (the_shell->current_process > 0) {
    kill(the_shell->current_process, SIGINT);
  }
  print_prompt(the_shell);
}

// ----------------------------------------------------------------------
//  API
// ----------------------------------------------------------------------

void shell_start() {

  Shell *shell = shell_new();

  String *line = string_new();
  

  signal(SIGQUIT, freeze_process);
  signal(SIGINT, kill_process);

  while (true) {
    print_prompt(shell);
	string_clear(line);
	
	if (read_line(line, stdin) == -1) {
	  printf("\n");
	  break;
	}
	string_trim(line);

    Vector *commands = vector_new(command_destroy);
    parse(line, commands);
	// shell_cd(shell, line);
    vector_destroy(commands);
  }

  string_destroy(line);
  shell_destroy(shell);
}

void shell_resolve_variable(const String *name, String *value) {
  for (size_t i = 0; i < vector_size(the_shell->variable_name); i++) {
    if (string_equal(name, vector_at(the_shell->variable_name, i))) {
      string_append(value, vector_at(the_shell->variable_value, i));
      return ;
    }
  }
  string_append(value, getenv(string_raw(name)));
}

void print_env_variables() {
  info("Environment Variables:");
  // TODO
}

void set_shell_variable(const String *name, const String *value) {
  for (size_t i = 0; i < vector_size(the_shell->variable_name); i++) {
    if (string_equal(name, vector_at(the_shell->variable_name, i))) {
      string_copy(vector_at(the_shell->variable_value, i), value);
      return ;
    }
  }
  vector_push(the_shell->variable_name, string_clone(name));
  vector_push(the_shell->variable_value, string_clone(value));
}

void set_env_variable(const String *name, const String *value) {
  setenv(string_raw(name), string_raw(value), 1);
}

void register_job(pid_t pid, bool is_foreground) {
  pid_t *pid_pointer = malloc(sizeof(pid_t));
  *pid_pointer = pid;
  list_push_back(the_shell->jobs, pid_pointer);

  if (is_foreground) {
    the_shell->current_process = pid;
    waitpid(pid, NULL, 0);
  }
}

