#include <stdio.h>
#include "dbg.h"
#include "ex22.h"

// note that Zed uses caps for global variables. I'm not sure if this is
// standard C style. He's illustrating a couple of patterns here...

// If you want to expose global
// data, you could expose it directly, as he's doing with THE_SIZE.

int THE_SIZE = 1000;

// An alternative is to have a global variable  and expose it via getter and
// setter functions, as he's doing with THE_AGE

int THE_AGE = 37;

int get_age() {
    return THE_AGE;
}

void set_age(int age) {
    THE_AGE = age;
}

// Depending on the context, you can also use a static local variable. This
// works whenever the method that modifies state can be the same method that
// generates output.

double update_ratio(double new_ratio) {
    static double ratio = 1.0;
    double old_ratio = ratio;
    ratio = new_ratio;
    return old_ratio;
}

void print_size() {
    log_info("I think size is: %d", THE_SIZE);
}
