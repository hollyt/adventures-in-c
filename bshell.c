/*for get_current_dir_name()*/
#define _GNU_SOURCE

#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> /*testing*/

void init_shell();
void commands(void);
char **parse_args(char *line);
int launch_process(char **args);
int execute(char **args);
/* SHELL BUILTINS */
int num_builtins();
int bshell_echo();
int bshell_exit();
int bshell_cd(char **args);
int bshell_help();

/*map shell builtin names to their functions*/
char *bshell_builtins[] = {
    "exit",
    "logout",
    "echo",
    "cd",
    "help"
};

int (*builtin_funcs[]) (char **args) = {
    &bshell_exit,
    &bshell_exit,
    &bshell_echo,
    &bshell_cd,
    &bshell_help
};

/*for process control*/
struct termios shell_tmodes;

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
    int shell_terminal,
        shell_is_interactive;

    /*set new session id for the shell*/
    //setsid();    
    /*make sure shell is foreground process*/
    shell_terminal = STDIN_FILENO;
    shell_is_interactive = isatty(shell_terminal);
    if (shell_is_interactive) {
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
}

void commands() {
    char *line = NULL;
    char **args = NULL;
    char *cwd = NULL;
    char **iterator = NULL;
    size_t len = 0;
    int i = 0,
        status = 0;

    do {
        /*cwd = get_current_dir_name();*/
        printf("(>**)> ");
        getline(&line, &len, stdin);
        /*replace newline with null char*/
        for (i = len-1; i >= 0; i--) {
            if (line[i] == '\n') {
                line[i] = '\0';
            }
        }
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
    /*TODO: implement pipe trick as a semaphore*/
    int foreground = 1;

    /*must fork & exec a new process to execute the command*/
    pid_t pid, wpid, pgid, shellpid, newpid;

    /*get the shell's process group id*/
    shellpid = tcgetpgrp(1);

    pid = fork();
    if (pid == 0) { /*fork was successful - child process*/
        /*setpgid(pid,0);*/

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
        wpid = waitpid(pid, NULL, 0);
    }
    return 1;
}

int execute(char **args) {
    int status,
        i;
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

/* SHELL BUILTINS */
int num_builtins() {
    return sizeof(bshell_builtins) / sizeof(char *);
}

int bshell_exit() {
    /*all commands but exit return 1 to continue the loop*/
    return 0;
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

int bshell_cd(char **args) {
    if (chdir(args[1]) == -1) {
        perror("bshell");
    }
    return 1;
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
    return 1;
}
