#ifndef SHELL_H
#define SHELL_H

#include <unistd.h>
#include <stdbool.h>

#include "vector.h"
#include "str.h"

typedef struct Shell Shell;

void shell_start();

void register_job(pid_t pid, bool is_foreground);

// Find the value of variable `name' in shell variables.
// If nothing found, try to query environment variables.
// 
void shell_resolve_variable(const String *name, String *value);

void print_env_variables();

void set_shell_variable(const String *name, const String *value);

void set_env_variable(const String *name, const String *value);

void freeze_process(int signal);

void kill_process(int signal);

#endif
