#include <stdio.h>

int main(int argc, char* argv[]) {
    int ages[] = {23, 43, 12, 89, 2};
    char *names[] = {
        "Alan", "Frank",
        "Mary", "John", "Lisa"
    };

    int count = sizeof(ages) / sizeof(int);
    
    int i;
    for (i = 0; i < count; i++) {
        printf("%s is %d years old.\n", names[i], ages[i]);
    }
    printf("------\n");

    // define pointers. Note that arrays are castable to pointers
    int *cur_age = ages;
    char **cur_name = names;

    // redo the same loop using pointers, with array-style syntax
    for (i = 0; i < count; i++) {
        printf("%s is %d years old.\n", cur_name[i], cur_age[i]);
    }
    printf("------\n");

    // redo the same loop using pointer-style syntax. Note that the + operator
    // knows the size of the target datatype, so `+ 1` works in general.
    for (i = 0; i < count; i++) {
        printf("%s is %d years old.\n", *(cur_name + i), *(cur_age + 1));
    }
    printf("------\n");

    // redo it again. don't write code this way, but be aware you may see it.
    for (cur_name = names, cur_age = ages; (cur_age - ages) < count;
           cur_name++, cur_age++) {
        printf("%s is %d years old.\n", *cur_name, *cur_age);
    }
    printf("------\n");


    return 0;
}
