// Exercise 27 is mostly not code, it's Zed's loose set of 'rules' for
// thinking about defensive programming. So you should read
// http://c.learncodethehardway.org/book/ex27.html repeatedly!
#include <stdio.h>
#include <assert.h>
#include "dbg.h"

/* unsafe string copy. Assumes (1) `from` is '\0' terminated and (2) that
 * we know for sure `to` is big enough to handle the copy. Easily leads to
 * infinite loops and/or buffer overflows and/or segfaults.
 * Note there is no return code. */
void copy(char *to, char *from) {
    int i = 0;
    while((to[i] = from[i]) != '\0') {
        i++;
    }
}


/* A much safer copy function. We force the user to specify the lengths of
 * the cstrings (they might do it wrong, but at least we force them to think
 * about it... and in some contexts e.g. bstrings this can be almost
 * guaranteed), and stay safe given those bounds.
 * ...
 * We also return the number of characters copied, and -1 if we catch an
 * error.
 * ...
 * The semantics are kind of weird though. from_len refers to the length of
 * `from` *not* including the '\0', wherease to_len refers to the length of
 * `to` *including* the '\0'. My impression is that based on experience,
 * Zed handles potential off-by-one mistakes by just allocating an extra
 * byte and assuming the worst. (He did something similar in a prev exercise
 * when calling a library function, where he gave it an uneeded extra byte).
 */
int safercopy(int from_len, char *from, int to_len, char *to) {
    // Zed likes using asserts to check for critical errors, like NULL
    // pointers, in self-contained programs. In a library he would instead use
    // an error code, log a warning/error, and not abort, but in your own code
    // this works because assert will abort, and the program will print this
    // line of code to stderr, which lets you see the message.
    assert(from != NULL && to != NULL && "from and to can't be NULL");
    // return an error code for the other obvious error.
    // ..Zed prefers aborting on null pointer outside libraries because if you
    // don't catch that mistake and abort here, it's likely to cause worse
    // problems somewhere else. Negative lens are less bad, so an error code
    // makes more sense.
    // Note that we require to_len to be > 0 b/c we need room for the '\0'.
    if (to_len <= 0 || from_len < 0) return -1;

    int i = 0;
    int max = from_len > to_len - 1 ? to_len - 1 : from_len;

    // remember what Jesse said about NASA: a while loop you can't guarantee
    // is finite from *inside* the function, regardless of inputs, is illegal.
    for (i = 0; i < max; i++) {
        to[i] = from[i];
    }
    to[to_len - 1] = '\0';

    return i;
}

int main(int argc, char *argv[])
{
    // careful to understand why we can get these sizes
    char from[] = "0123456789";
    int from_len = sizeof(from);

    // notice that it's 7 chars + \0
    char to[] = "0123456";
    int to_len = sizeof(to);

    debug("Copying '%s':%d to '%s':%d", from, from_len, to, to_len);

    int rc = safercopy(from_len, from, to_len, to);
    check(rc > 0, "Failed to safercopy.");
    check(to[to_len - 1] == '\0', "String not terminated.");

    debug("Result is: '%s':%d", to, to_len);

    // now try to break it
    rc = safercopy(from_len * -1, from, to_len, to);
    check(rc == -1, "safercopy should fail #1");
    check(to[to_len - 1] == '\0', "String not terminated.");

    rc = safercopy(from_len, from, 0, to);
    check(rc == -1, "safercopy should fail #2");
    check(to[to_len - 1] == '\0', "String not terminated.");

    return 0;

error:
    return 1;
}
