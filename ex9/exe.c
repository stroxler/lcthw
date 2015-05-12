#include <stdio.h>

int main(int argc, char* argv[]) {

    /*
     * What does this demonstrate? If you set an arrays size (you can also
     * not set the size and infer it from the literal) and the literal has
     * smaller size, it auto fills with zeros (which for char is the same
     * as '\0').
     *
     * The '\0' doesn't print if you use '%c'. Also, because it fills with
     * '\0' you are able to print as a string without fear of segfaulting.
     */
    int numbers[4] = {5};
    char name[4] = {'a'};
    printf("numbers: %d %d %d %d\n",
            numbers[0], numbers[1],
            numbers[2], numbers[3]);
    printf("name each: %c %c %c %c\n",
            name[0], name[1],
            name[2], name[3]);
    printf("name: %s\n", name);

    /*
     * You can also set things manually
     */
    numbers[0] = 1; numbers[1] = 2; numbers[2] = 3; numbers[3] = 4;
    name[0] = 'Z'; name[1] = 'e'; name[2] = 'd'; name[3] = '\0';
    printf("numbers: %d %d %d %d\n",
            numbers[0], numbers[1],
            numbers[2], numbers[3]);
    printf("name each: %c %c %c %c\n",
            name[0], name[1],
            name[2], name[3]);
    printf("name: %s\n", name);

    /*
     * And you can get the same affect by assigning a string literal to
     * a pointer. Watch out in future examples: when you assign a pointer
     * to a string literal, it lives in read-only memory; attempts to modify
     * it will segfault. This is a quirk of C, because if you do the same
     * thing to an array it lives in stack memory and is writeable.
     */
    char *another = "Zed";
    printf("another: %s\n", another);
    printf("another each: %c %c %c %c\n",
            another[0], another[1],
            another[2], another[3]);

    return 0;
}
