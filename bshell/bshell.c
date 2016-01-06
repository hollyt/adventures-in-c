#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void commands(void);
char **parse_args(char *line);

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
    char *status = NULL;
    char **iterator = NULL;
    size_t len = 0;
    int i = 0;

    do {
        // kirby prompt
        printf("(>**)> ");
        getline(&line, &len, stdin);
        args = parse_args(line);
        /*
        status = execute(args);
        */
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
