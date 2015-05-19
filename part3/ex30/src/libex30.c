#include <stdio.h>
#include <ctype.h>
#include "dbg.h"


int print_a_message(const char *msg) {
    printf("A STRING: %s\n", msg);
    return 0;
}

// note: this is how Zed coded this... he doesn't like that it
// has no buffer size check though. You should really fix it to take
// a length, and convert until you either hit '\0' or length - 1
int uppercase(const char *msg) {
    int i = 0;
    for(i = 0; msg[i] != '\0'; i++) {
        printf("%c", toupper(msg[i]));
    }
    printf("\n");
    return 0;
}

// same issue
int lowercase(const char *msg) {
    int i = 0;
    for(i = 0; msg[i] != '\0'; i++) {
        printf("%c", tolower(msg[i]));
    }
    printf("\n");
    return 0;
}

int fail_on_purpose(const char *msg) {
    return 1;
}
