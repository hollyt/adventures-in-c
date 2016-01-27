#include "builtins.h"

/* SHELL BUILTIN FUNCTIONS */

int bshell_cd(char **args) {
    const char *path = args[1];
    char *home_env = getenv("HOME");

    if (home_env) {
        /*tilde expansion*/
        if (strchr(path, '~')) {
            strcat(home_env, path+1);
            if (chdir(home_env) == -1) {
                perror("bshell");
            }
        }
    }

    else if (chdir(args[1]) == -1) {
        perror("bshell");
    }
    return 1;
}

int bshell_echo(char **args) {
    int i;

    /*start @ 1st element becuase 0th is 'echo'*/
    for (i = 1; args[i] != NULL; i++) {
        if (i == 1) {
            printf("%s", args[i]);
        }
        else {
            printf(" %s", args[i]);
        }
    }
    printf("\n");

    return 1;
}

int bshell_exit() {
    /*all commands but exit return 1 to continue the loop*/
    return 0;
}

int bshell_help() {
    int i;

    printf("                               ");
    printf("BSHELL - BABY SHELL\n");
    printf("                              ");
    printf("=====================\n");
    printf("BUILTINS:\n");
    for (i = 0; i < num_builtins(); i++) {
        printf("%s\n", bshell_builtins[i]);
    }
}

/* Map builtin name to its function.*/
char *bshell_builtins[] = {
    "cd",
    "echo",
    "exit",
    "help",
    "logout"
};

int (*builtin_funcs[]) (char **args) = {
    &bshell_cd,
    &bshell_echo,
    &bshell_exit,
    &bshell_help,
    &bshell_exit
};

int num_builtins() {
    return sizeof(bshell_builtins) / sizeof(char *);
}
