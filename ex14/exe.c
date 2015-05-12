#include <stdio.h>
#include <ctype.h> // the isX functions live here

// forward declarations
void print_arguments(int argc, char *argv[]);
int can_print_it(char ch);
void print_letters(char arg[]);

int main(int argc, char* argv[]) {
    print_arguments(argc, argv);
    return 0;
}

void print_arguments(int argc, char *argv[]) {
    int i = 0;
    for (i = 0; i < argc; i++) {
        print_letters(argv[i]);
    }
}

void print_letters(char arg[]) {
    int i = 0;
    for (i = 0; arg[i] != '\0'; i++) {
        char ch = arg[i];
        if (can_print_it(ch)) {
            // note %c and %d *both* work for char
            printf("'%c' == %d | ", ch, ch);
        }
    }
    printf("\n");
}

int can_print_it(char ch) {
    return isalpha(ch) || isblank(ch) || isdigit(ch);
}
