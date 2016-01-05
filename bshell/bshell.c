#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void commands(void);

int main(int argc, char **argv) {

    // config

    // command loop
    commands();

    // cleanup

    return 0;
}

void commands() {
    // starting out small for now
    int MAX = 100;
    char line[MAX+1];
    char **args;
    char *status;

    do {
        // kirby prompt
        printf("(>**)> ");
        fgets(line, MAX, stdin);
        /*
        args = parse_args(line);
        status = execute(args);

        free(line);
        free(args);
        */
    } while(status);
}
