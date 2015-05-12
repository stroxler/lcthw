#include <stdio.h>

int main(int argc, char* argv[]) {
    int i = 0;
    char *usage = "Usage: %s <arg1> [<arg2> <arg3>] (error: %s)\n";

    if (argc == 1) {
        printf(usage, argv[0], "Too few arguments");
        return 1;
    } else if (argc > 4) {
        printf(usage, argv[0], "Too many arguments");
        return 1;
    } else {
        for (i = 0; i < argc; i++) {
            printf("Argument %d: %s\n", i, argv[i]);
        }
    }
    return 0;
}
