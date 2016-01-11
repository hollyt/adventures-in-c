#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void commands(void);
char **parse_args(char *line);
int launch_process(char **args);
int execute(char **args);
/* SHELL BUILTINS */
int num_builtins();
int bshell_exit();

/*map shell builtin names to thier functions*/
char *bshell_builtins[] = {
    "exit"
};

int (*builtin_funcs[]) (char **args) = {
    &bshell_exit
};

/* MAIN */
int main(int argc, char **argv) {

    /* config */

    /* command loop */
    commands();

    /* cleanup */
    
    return 0;
}

void commands() {
    char *line = NULL;
    char **args = NULL;
    char **iterator = NULL;
    size_t len = 0;
    int i = 0,
        status = 0;

    do {
        /*kirby prompt*/
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
        for (i = 0; args[i] != NULL; i++) {
            free(args[i]);
        }
        free(args);
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
        //check for successful reallocation
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
    /*must fork & exec a new process to execute the command*/
    pid_t pid, wpid;

    pid = fork();
    if (pid == 0) { /*fork was successful - child process*/
        /*try to execute the command*/
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
