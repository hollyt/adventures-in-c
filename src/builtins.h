#ifndef  BUILTINS_H
#define BUILTINS_H

#include <stdio.h>

/* SHELL BUILTINS */
int bshell_cd(char **args);
int bshell_echo();
int bshell_exit();
int bshell_help();
int num_builtins();

/*map shell builtin names to their functions*/
extern char *bshell_builtins[];

extern int (*builtin_funcs[]) (char **args);

#endif
