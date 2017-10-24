#include "shell.h"
#include "util.h"

#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

// ----------------------------------------------------------------------
//  Internal
// ----------------------------------------------------------------------

typedef struct shell shell;

struct shell {
  String *pwd;
};

static shell *shell_new() {
  shell *shell = malloc(sizeof(struct shell));

  // Set home directory.
  shell->pwd = string_from(getenv("HOME"));
  if (string_empty(shell->pwd)) {
	error("Cannot locate $HOME directory.");
	string_set(shell->pwd, "/");
  }

  // Set current directory
  if (chdir(string_raw(shell->pwd)) == -1) {
	error(strerror(errno));
  }
  
  info("Starting shell environment...");
  info("Current working directory: %s", string_raw(shell->pwd));
  return shell;
}

static void shell_destroy(shell *shell) {
  free(shell);
}

static void shell_cd(shell *shell, String *dir) {
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


// ----------------------------------------------------------------------
//  API
// ----------------------------------------------------------------------

void shell_start() {

  shell *shell = shell_new();

  String *line = string_new();
  
  while (true) {
	printf("%s $ ", string_raw(shell->pwd));
	string_clear(line);
	
	if (read_line(line, stdin) == -1) {
	  printf("\n");
	  break;
	}
	string_trim(line);
	printf("Your input: %s\n", string_raw(line));
	shell_cd(shell, line);
  }

  string_destroy(line);
  shell_destroy(shell);
}

