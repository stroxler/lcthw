#include <stdio.h>

int main(int argc, char* argv[]) {
    int i = 0;

    // print the name of the executable called
    printf("You called: %s\n", argv[0]);

    // Go through the command line args, skipping args[0]
    for(i = 1; i < argc; i++) {
        printf("arg %d: %s\n", i, argv[i]);
    }

    // in definitions, * binds before []. So this is an array of char*, not
    // a pointer to char[]
    char *states[] = {
        "Arizona",
        "California",
        "Georgia"
    };
    int num_states = 3;
    for (i = 0; i < num_states; i++) {
        printf("State %d: %s\n", i, states[i]);
    }

    return 0;
}
