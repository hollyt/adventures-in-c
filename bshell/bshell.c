#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void commands(void);
char *parse_args(char *line);

// starting small for now
int MAX = 100;

int main(int argc, char **argv) {

    // config

    // command loop
    commands();

    // cleanup

    return 0;
}

void commands() {
    char *line = NULL;
    size_t len = 0;
    char **args;
    char *status;

    do {
        // kirby prompt
        printf("(>**)> ");
        getline(&line, &len, stdin);
        free(line); 
        /*
        args = parse_arg(line);
        status = execute(args);

        free(args);
        */
    } while(status);
}

char *parse_args(char *line) {
    int i = 0;
    const char delims[2] = " ";
    // for testing purposes ...
    int SIZE = 10;
    char *args[SIZE+1];

    // strtok

    for (i = 0; i < MAX; i++) {
        
    }   
}
