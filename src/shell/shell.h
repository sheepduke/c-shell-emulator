#ifndef SHELL_H
#define SHELL_H

#include <unistd.h>
#include <stdbool.h>

typedef struct Shell Shell;

void shell_start();

void register_job(pid_t pid, bool is_foreground);

void freeze_process(int signal);

void kill_process(int signal);

#endif
