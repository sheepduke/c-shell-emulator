# About Unix Shell
A shell is rather simple: it takes user input and parse it, then perform accordingly. Beneath the surface, it uses system calls to implement system related functions.


# About this project
So the goal of this project is to implement a shell that can parse user input and behaves like a Linux bash shell.

It is not finished, and I am not intended to finish it, for mainly 2 reasons:

1. The remaining part of this project is somehow dirty work and not educational any more.

2. Since hardest parts are resolved, it becomes boring for me to accomplish remaining functions.

But maybe the code is helpful for you if you need some tips.

# Implementation
## Variable Expanding
Variables are divided into two categories: environment variables and shell variables. 

Environment variables are introduced by using `export` and shell variables are defined by `VAR=VALUE`.

## Calling External Commands
You can use `fork` to run external commands. `fork` creates a new sub-process that runs code after the point of `fork`.

Return value of `fork` is different for parent and child process. This can be used to distinguish parent and child processes.

Please keep in mind that child process shared no information with parent after the system call. You cannot define a variable in parent process after `fork` call and expect that the child process can access it.

## I/O Redirection
I/O redirection is down by using system call `dup2` that can "copy" a file descriptor to another one. For example, if you `dup2` a file descriptor `fd` to `stdout`, the `stdout` is overwritten by `fd` and all the standard output will be redirected to file indicated by `fd`.

## Pipeline
The hardest part in pipeline is that you have to carefully control the data flow between piped commands. Let us say you have three commands: `A`, `B` and `C`. So you have to "connect" output of `A` to input of `B` and output of `B` to input of `C`.

This can be done in 2 steps:

1. A pipe should be created by calling `pipe`. A pipe contains two ports: input and output. You have to make sure that irrelevant port is closed and the direction of flow is correct.

2. You should use `dup2` to "bind" output port to corresponding input port.

## Foreground And Background Command
"Foreground"" means that the shell will wait until command sequence finish. "Background" means the reverse, the shell works in a non-blocking way, it may continue to accept user input.

System call `waitpid` is used to "wait" for sub-process to finish, thus can be used to implement foreground commands.

# Remaining Work
- Improve/fix parser rules of form VAR=VALUE.
- Finish syntax parser.
- Maintain return code `$?`.
- Implement IO redirection and connect it to commands.
