#include <stdio.h>
#include <string.h>
#include "dbg.h"


int normal_copy(char *from, char *to, int count) {
    int i = 0;
    for(i = 0; i < count; i++) {
        to[i] = from[i];
    }
    return i;
}

// Note: I'm pretty sure that both duffs_device and zeds_device fail if
// count is zero.
int duffs_device(char *from, char *to, int count) {
    {
        int n = (count + 7) / 8;
        switch(count % 8) {
            case 0: do { *to++ = *from++;
                        case 7: *to++ = *from++;
                        case 6: *to++ = *from++;
                        case 5: *to++ = *from++;
                        case 4: *to++ = *from++;
                        case 3: *to++ = *from++;
                        case 2: *to++ = *from++;
                        case 1: *to++ = *from++;
                    } while(--n > 0);
        }
    }
    return count;
}

int zeds_device(char *from, char *to, int count) {
    {
        int n = (count + 7) / 8;
        switch(count % 8) {
            case 0:
            again: *to++ = *from++;
            case 7: *to++ = *from++;
            case 6: *to++ = *from++;
            case 5: *to++ = *from++;
            case 4: *to++ = *from++;
            case 3: *to++ = *from++;
            case 2: *to++ = *from++;
            case 1: *to++ = *from++;
                    if(--n > 0) goto again;
        }
    }
    return count;
}

/* return 1 if valid, 0 if not. */
int is_valid_copy(char *data, int count, char expects) {
    int i = 0;
    for(i = 0; i < count; i++) {
        if(data[i] != expects) {
            log_err("[%d] %c != %c", i, data[i], expects);
            return 0;
        }
    }
    return 1;
}


int main(int argc, char *argv[]) {
    char from[1000] = {'a'};
    char to[1000] = {'c'};
    int rc = 0;

    // setup the from to have some stuff
    // ... void *memset(void *s, int c, size_t n) sets the first n bytes
    //     after location `s` to be a copy of the (first byte of ?) c, and
    //     returns `s` (but in most cases I think people ignore the return val)
    //     It's kind of weird that it uses an int, because before memset
    //     does it's work, the int gets cast to an unsigned char. Meh.
    memset(from, 'x', 1000);
    // set it to a failure mode
    memset(to, 'y', 1000);
    check(is_valid_copy(to, 1000, 'y'), "Not initialized right.");

    // use normal copy to 
    rc = normal_copy(from, to, 1000);
    check(rc == 1000, "Normal copy failed: %d", rc);
    check(is_valid_copy(to, 1000, 'x'), "Normal copy failed.");

    // reset
    memset(to, 'y', 1000);

    // duffs version
    rc = duffs_device(from, to, 1000);
    check(rc == 1000, "Duff's device failed: %d", rc);
    check(is_valid_copy(to, 1000, 'x'), "Duff's device failed copy.");

    // reset
    memset(to, 'y', 1000);

    // my version
    rc = zeds_device(from, to, 1000);
    check(rc == 1000, "Zed's device failed: %d", rc);
    check(is_valid_copy(to, 1000, 'x'), "Zed's device failed copy.");

    return 0;
error:
    return 1;
}
