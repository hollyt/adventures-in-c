#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void commands(void);
char **parse_args(char *line);
void launch_process(char **args);
void execute(char **args);

int main(int argc, char **argv) {

    // config

    // command loop
    commands();

    // cleanup

    return 0;
}

void commands() {
    char *line = NULL;
    char **args = NULL;
    char **iterator = NULL;
    size_t len = 0;
    int status = 0;

    do {
        // kirby prompt
        printf("(>**)> ");
        getline(&line, &len, stdin);
        args = parse_args(line);
        execute(args);
    } while(1);
}

char **parse_args(char *line) {
    // tokenize the string on whitespace only for now
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
    // end in null
    args = realloc(args, sizeof(char*) * n_spaces+1);
    args[n_spaces] = NULL;

    return args;   
}

void launch_process(char**args) {
    // must fork & exec a new process to execute the command
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
}

void execute(char **args) {
    /* check for shell builtins*/
 
    launch_process(args);
}
