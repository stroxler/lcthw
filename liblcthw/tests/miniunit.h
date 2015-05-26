#undef NDEBUG
#ifndef _miniunit_h
#define _miniunit_h

#include <stdio.h>
#include <stdlib.h>
#include <lcthw/dbg.h>

// note that dbg.h goes in < > because it will get included from src on
// the include path. I may change this b/c it's making vim syntastic go
// nuts.

// note that debug is *always* turned off when you include this header

// the #var syntax is the "Stringizing operator". It takes the literal
// passed in var and wraps it in ""s -- it's needed because if you tried to
// put "test" in quotes, the preprocessor would *not* replace it due to the ""s

int _mu_tests_run;

#define mu_suite_start() char *_mu_message = NULL

#define mu_assert(test, message) if (!(test)) { \
    log_err(message); \
    return message; }


#define mu_run_test(test) debug("\n----%s", " " #test); \
    _mu_message = test(); _mu_tests_run++; if (_mu_message) return _mu_message;

// note that a call to this macro takes the place of main
//    how do you use it? You make a master test function to run all tests,
//    and inside it you repeatedly call mu_run_test with the name of individual
//    test functions (they themselves could delegate to more functions if
//    you wanted). If anything fails along the way, the failure message gets
//    propogated all the way up and you get a test failure.
//      ... as with the test run shell script, a downside of this is that
//    the very first failed test kills the whole process, so you won't see
//    a full set of failures when you introduce a regression. But the flip
//    side is that it's very simple and lightweight.
//
// the argc += 1 is just there to trick the C compiler, which for some reason
// thinks unused vars are ok in main but not if it comes from a macro, into
// not creating unused variable warnings.
#define RUN_TESTS(name) int main(int argc, char *argv[]) { \
    argc += 1; \
    debug( "------> RUNNING: %s\n", argv[0]); \
    printf("------> RUNNING: %s\n", argv[0]); \
    char *result = name(); \
    if (result != 0) { \
        printf("FAILED: %s\n", result); \
    } \
    else { \
        printf("ALL TESTS PASSED\n"); \
    } \
    printf("Tests run: %d\n", _mu_tests_run); \
    exit(result != 0); \
}

#endif
