#include <stdio.h>

/* this is an exact rewrite of ex10 using while instead of for */
int main(int argc, char* argv[]) {

    printf("name of executable: %s\n", argv[0]);

    int i = 1;
    while (i < argc) {
        printf("arg %d: %s\n", i, argv[i]);
        i++;
    }

    char *states[] = {
        "Arizona",
        "California",
        "Georgia"
    };
    int num_states = 3;
    
    i = 0;
    while (i < num_states) {
        printf("state %d: %s\n", i, states[i]);
        i++;
    }
    return 0;
}
