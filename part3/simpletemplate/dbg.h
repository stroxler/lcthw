#ifndef __dbg_h__
#define __dbg_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>

// General notes: you could add __func__ to these print statements to get func
// name. If you want nice function names that include class/namespace/inputs in
// C++ where overloading is an issue, use __PRETTY_FUNCTION__ instead.
// The ##__VA_ARGS__ is CPP's way of inserting whatever was in the ...

// use NDEBUG, by #defining it before including or via -DNDEBUG, to turn off
// debug messages. This lets you litter dev code with messages and turn them
// off in a production build.
//
// note that there's not ; after debug, which lets us call it like a function
#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

// clean_error() 'returns' a pretty-printed errno-related string
// note that we use it in macros below; macros evaluate recursively.
#define clean_errno() (errno == 0 ? "None" : strerror(errno))

// log_err logs an error message, which may or may not be related to an errno
#define log_err(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

// log_warn is the same as log_err; use it for less serious situations
#define log_warn(M, ...) fprintf(stderr, "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

// info is similar, but not for error handling (so no errno checks)
#define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

// sentinel is a readable shortcut for generic error handling
#define sentinel(M, ...)  { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

// check is the same as sentinel except it first checks a condition
// note that we don't put a ; after the sentinel call!
#define check(A, M, ...) if(!(A)) sentinel(M, ##__VA_ARGS__)

// shortcut for calling check after malloc and printing a standard message
#define check_mem(A) check((A), "Out of memory.")

// similar to check, except it only prints if we are in debug mode
#define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; }

#endif
