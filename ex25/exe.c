#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "dbg.h"

#define MAX_DATA 100

// think of this char ** as really a pointer to a char *, not a double-array.
int read_string(char **out_string, int max_buffer) {
    // note we use calloc, which means we don't have to add '\0'
    //   in reality fgets adds a null for us, so malloc would have been ok.
    *out_string = calloc(1, max_buffer);
    check_mem(out_string);
    
    // note that Zed avoids using gets... for good reason!
    char *result = fgets(*out_string, max_buffer, stdin);
    check(result != NULL, "Input error.");

    return 0;

error:
    // note that because out_string lives outside this function, it isn't
    // enough just to free it, it needs to be set to NULL in cleanup.
    if (*out_string) free(*out_string);
    *out_string = NULL;
    return -1;
}

// returns an error code: 0 for success, 1 for failure.
int read_int(int *out_int) {
    char *input = NULL;
    int rc = read_string(&input, MAX_DATA);
    check(rc == 0, "Failed to read Number");
    *out_int = atoi(input);
    free(input);
    return 0;
error:
    if (input) free(input);
    return -1;
}

/* similar to scanf, except it only reads %c, %d, and %s.
 * returns 0, or -1 as error code.
 * note that if you use %s, then the char ** input must be preceded by an
 * int giving a max buffer (this is not the case for the scanf functions,
 * but makes the demo implementation of read_string above much simpler).
 */
int read_scan(const char * fmt, ...) {
    // vars used to handle flow
    int i = 0, rc = 0;
    // vars used to handle output
    int *out_int = NULL;
    char *out_char = NULL;
    char **out_string = NULL;
    int max_buffer = 0;

    // handle input. How does this work? the code needs to know where in
    // ram to look for the arguments, and C doesn't have built-in logic for
    // it's argument lists to tell it this (there's no reason it couldn't be
    // built-in, it just isn't). So, you call va_start to initialize argp, and
    // the second format needs to be the last named input. By looking at the
    // second arg and its type, va_start can figure out where in ram to look.
    // Note that va_start is a macro, not a function; it can't be a function
    // because it's actually inferring the type of the 2nd argument!
    va_list argp;
    va_start(argp, fmt);

    for (i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {
            i++;
            switch(fmt[i]) {
                case '\0':
                    sentinel("Invalid format, you ended with %%");
                    break;
                case 'd':
                    out_int = va_arg(argp, int *);
                    // we aren't passing fmt here: remember fmt is just the
                    // format, the actual input is stdin!
                    rc = read_int(out_int);
                    check(rc == 0, "Failed to read int.");
                    break;
                case 'c':
                    out_char = va_arg(argp, char *);
                    *out_char = fgetc(stdin); // getchar() would also work
                    break;
                case 's':
                    max_buffer = va_arg(argp, int);
                    out_string = va_arg(argp, char **);
                    rc = read_string(out_string, max_buffer);
                    check(rc == 0, "Failed to read string.");
                    break;
                default:
                    sentinel("Invalid format, only %%s, %%c, %%d supported.");
            }
        } else {
            // need to eat up a character of stdin, looking for the next %
            // because non-control chars in fmt correspont 1-1 with chars in
            // the input.
            fgetc(stdin);
        }
        // check whether we hit EOF in stdin (otherwise we could get ugly
        // errors). If this occurs, the input ended too early and it's an err.
        // I'
        check(!feof(stdin) && !ferror(stdin), "Input error.");
    }
    // it's possible that va_start and/or one of the va_arg calls might have
    // allocated resources. Every function that uses va_start or va_copy should
    // include a va_end in cleanup.
    va_end(argp);
    return 0;

error:
    // it feels like we're missing a free here, but actually we didn't alloc
    // anywhere in this function (other than possibly in va_X).
    va_end(argp);
    return -1;
}


int main(int argc, char *argv[]) {
    char *first_name = NULL;
    char initial = ' ';
    char *last_name = NULL;
    int age = 0;

    printf("What's your first name? ");
    int rc = read_scan("%s", MAX_DATA, &first_name);
    check(rc == 0, "Failed first name.");

    // note the '\n'.
    //  -Do you understand why this is here?
    //  -Why isn't it here in the %s version? [answer - it gets absorbed]
    //  -Why isn't it here in the %d version? [answer - read_string takes the
    //                                                  whole line]
    printf("What's your initial? ");
    rc = read_scan("%c\n", &initial);
    check(rc == 0, "Failed initial.");

    printf("What's your last name? ");
    rc = read_scan("%s", MAX_DATA, &last_name);
    check(rc == 0, "Failed last name.");

    printf("How old are you? ");
    rc = read_scan("%d", &age);

    printf("---- RESULTS ----\n");
    printf("First Name: %s", first_name);
    printf("Initial: '%c'\n", initial);
    printf("Last Name: %s", last_name);
    printf("Age: %d\n", age);

    free(first_name);
    free(last_name);
    return 0;
error:
    return -1;
}
