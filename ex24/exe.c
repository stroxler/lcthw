#include <stdio.h>
#include "dbg.h"

#define MAX_DATA 100

typedef enum EyeColor {
    BLUE_EYES, GREEN_EYES, BROWN_EYES, BLACK_EYES, OTHER_EYES
} EyeColor;

const char *EYE_COLOR_NAMES[] = {
    "Blue", "Green", "Brown", "Black", "Other"
};

typedef struct Person {
    int age;
    char first_name[MAX_DATA];
    char last_name[MAX_DATA];
    EyeColor eyes;
    float income;
} Person;

// Demonstrate getting input with fgets and fscanf.
// Use these in preference to the standard-input-specific functions because
// (1) there are some weird differences and these are safer: e.g. gets doesn't
//     have a limit on the string size, so there's no way to prevent overflow!
// (2) it means your code can be adapted easily to get input from a different
//     file descriptor; you aren't locked into console input.
int main(int argc, char* argv[]) {
    Person you = { .age = 0 };
    int i = 0;

    // fgets(char *s, int n, FILE *f) returns s if successfull. Otherwise
    // it returns null. This is its [kind of strange] way of indicating
    // success / failure. The `in` variable is here so we can check for errors.
    //
    // It adds a null-terminating character. According to the docs, n is the
    // max number of characters it will use *including* the '\0', so I *think*
    // Zed was being overcautious when he used MAX_DATA - 1 everywhere
    char *in = NULL;

    printf("What's your first name?\n");
    in = fgets(you.first_name, MAX_DATA - 1, stdin);
    check(in != NULL, "Failed to read first name.");

    printf("What's your last name?\n");
    in = fgets(you.last_name, MAX_DATA - 1, stdin);
    check(in != NULL, "Failed to read last name.");

    // fscanf(File *f, char *fmt, type *ptr1, ...)
    // It returns 0 for error.
    printf("How old are you?\n");
    int rc = fscanf(stdin, "%d", &you.age);
    check(rc > 0, "You have to enter a number.");

    // float and int is pretty much the same
    printf("How much do you make in an hour?\n");
    rc = fscanf(stdin, "%f", &you.income);
    check(rc > 0, "Enter a floating point number.");

    // getting an enum value is usually more complicated, and requires a menu
    //
    // note that here we rely on the fact that OTHER_EYES is last in the enum
    // and that C guarantees you that enums count up from 0 in the order given
    printf("What color are your eyes?\n");
    for (i = 0; i < OTHER_EYES; i++) {
        printf("\t%d) %s\n", i+1, EYE_COLOR_NAMES[i]);
    }
    int eyes = -1;
    rc = fscanf(stdin, "%d", &eyes);
    check(rc > 0, "You have to enter a number");
    you.eyes = eyes - 1;
    check(you.eyes >= 0 && you.eyes <= OTHER_EYES, "That was not an option.");
    
    printf("----- RESULTS -----\n");

    printf("First Name: %s", you.first_name);
    printf("Last Name: %s", you.last_name);
    printf("Age: %d\n", you.age);
    printf("Eyes: %s\n", EYE_COLOR_NAMES[you.eyes]);
    printf("Income: %f\n", you.income);

    return 0;

error:
    // note that unlike in most situations, you don't need to clean up after
    // errors in main b/c you are exiting anyway.
    return -1;
}
