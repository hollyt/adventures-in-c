/*for get_current_dir_name()*/
#define _GNU_SOURCE

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "builtins.h"
#include "job_control.h"

void init_shell();
void commands(void);
char **parse_args(char *line);
int launch_process(char **args);
int execute(char **args);

int shell_terminal;

/* MAIN */
int main(int argc, char **argv) {
    init_shell();

    /* config */

    /* command loop */
    commands();

    /* cleanup */

    return 0;
}

/*initialize shell & set up signal & job control*/
/*www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html*/
void init_shell() {
    /*keep track of shell attributes*/
    pid_t shell_pgid;
    int shell_is_interactive;

    /*make sure shell is foreground process*/
    shell_terminal = STDIN_FILENO;
    while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp())) {
        kill (- shell_pgid, SIGTTIN);
    }
    /*ignore interactive and job-control signals so the
    shell doesn't kill its own process*/
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    /*put ourselves in our own process group*/
    shell_pgid = getpid();
    if (setpgid(shell_pgid, shell_pgid) < 0) {
        perror("Couldn't put the shell in its own process group.");
        exit(1);
    }

    /*grab control of the terminal*/
    tcsetpgrp(shell_terminal, shell_pgid);
    /*save default terminal attributes for shell*/
    tcgetattr(shell_terminal, &shell_tmodes);
}

void commands() {
    const char *prompt = "(>**)> ";
    char *line;
    char **args = NULL;
    int status = 1;

    do {
        line = readline(prompt);
        if (*line == EOF) { //EOF entered
            status = bshell_exit();
        }
        add_history(line);
        args = parse_args(line);
        status = execute(args);

        /*cleanup*/
        free(line);
        line = NULL;
    } while(status);
}

char **parse_args(char *line) {
    /*tokenize the string on whitespace only for now*/
    const char delim[2] = " ";
    char *token = NULL;
    char **args = malloc(sizeof **args);
    int n_spaces = 0;

    token = strtok(line, delim);
    while (token != NULL) {
        args = realloc(args, sizeof(char*) * ++n_spaces);
        /*check for successful reallocation*/
        if (args == NULL) {
            return NULL;
        }
        args[n_spaces-1] = token;
        token = strtok(NULL,delim);
    }
    /*end in null*/
    args = realloc(args, sizeof(char*) * n_spaces+1);
    args[n_spaces] = NULL;
    return args;
}

int launch_process(char**args) {
    pid_t pid, wpid, pgid;

    pid = fork();
    if (pid == 0) { /*fork was successful - child process*/
        setpgid(0,0);
        tcsetpgrp(shell_terminal, getpgrp());

        /*signal handling*/
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);

        if (execvp(args[0], args) == -1) {
            perror("bshell");
            exit(EXIT_FAILURE);
        }
    }
    else if (pid < 0) { /*fork failed*/
        perror("bshell");
    }
    else { /*fork was successful - parent process*/
        /* wait while child process executes the command*/
        setpgid(pid,pid);
        wpid = waitpid(pid, NULL, 0);
        tcsetpgrp(shell_terminal, getpgrp());
    }
    return 1;
}

int execute(char **args) {
    int status,
        i,
        infile,
        outfile,
        new_pipe[2];
    pid_t pid;

    /*check for shell builtins*/
    for (i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], bshell_builtins[i]) == 0){
            return (*builtin_funcs[i])(args);
        }
     }

    /*if not shell builtin, execute here*/
    status = launch_process(args);
    return status;
}
