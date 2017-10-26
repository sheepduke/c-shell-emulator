#include "shell.h"
#include "util.h"
#include "cmd_parser.h"

#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

// ----------------------------------------------------------------------
//  Internal
// ----------------------------------------------------------------------

typedef struct Shell Shell;

struct Shell {
  String *pwd;
};

static Shell *shell_new() {
  Shell *shell = malloc(sizeof(Shell));

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


// ----------------------------------------------------------------------
//  API
// ----------------------------------------------------------------------

void shell_start() {

  Shell *shell = shell_new();

  String *line = string_new();
  
  while (true) {
	printf("%s $ ", string_raw(shell->pwd));
	string_clear(line);
	
	if (read_line(line, stdin) == -1) {
	  printf("\n");
	  break;
	}
	string_trim(line);
    parse(line);
	// shell_cd(shell, line);
  }

  string_destroy(line);
  shell_destroy(shell);
}

