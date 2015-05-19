#include <dlfcn.h>
#include "miniunit.h"

typedef int (*lib_function)(const char *data);
char *lib_file = "build/libex30.so";
void *lib = NULL;

// check that dynamic linking works, and check an input/output pair
//    note that here we return 0 for error here. Why? Because the test
//    framework is assuming that we use true/false for pass/nopass.
//     ... I'd probably rather define PASS and FAIL in the header file for
//    clarity. I may do that in future examples.
int check_function(const char *func_to_run, const char *data, int expected) {
    lib_function func = dlsym(lib, func_to_run);
    check(func != NULL, "Did not find %s function in library %s: %s",
          func_to_run, lib_file, dlerror());
    int rc = func(data);
    check(rc == expected, "Function %s return %d for data: %s",
          func_to_run, rc, data);
    return 1;
error:
    return 0;
}

// test that we successfully can open the library
//    in what might be considered bad form, we are using side-effects
//    here... this test needs to run first.
// Note that passing tests return NULL.
char *test_dlopen() {
    lib = dlopen(lib_file, RTLD_NOW);
    mu_assert(lib != NULL, "Failed to open the library to test.");
    return NULL;
}

// test that the dynamically linked functions work
char *test_functions() {
    mu_assert(check_function("print_a_message", "Hello", 0),
                             "print_a_message failed");
    mu_assert(check_function("uppercase", "Hello", 0),
                             "uppercase failed");
    mu_assert(check_function("lowercase", "Hello", 0),
                             "lowercase failed");
    return NULL;
}

// test a function that's expected to fail
char *test_failures() {
    mu_assert(check_function("fail_on_purpose", "Hello", 1), 
                             "fail_on_purpose should fail.");
    return NULL;
}

// test that close is successful
char *test_dlclose() {
    int rc = dlclose(lib);
    mu_assert(rc == 0, "Failed to close lib.");
    return NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_dlopen);
    mu_run_test(test_functions);
    mu_run_test(test_failures);
    mu_run_test(test_dlclose);

    return NULL;
}

RUN_TESTS(all_tests);
