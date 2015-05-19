#include <stdarg.h>
#include "shell.h"
#include "dbg.h"

/* Top-level function for executing shells (I probably would have reversed
 * the names of Shell_exec, which calls Shell_run, and Shell_run).
 * ....
 * It initializes the apr pool and handles the varargs logic for
 * filling out the template with specific args. Shell_run is what actually uses
 * apr utilities to run the shell. */
int Shell_exec(Shell template, ...) {


    int rc = -1; // return code
    apr_status_t rv = APR_SUCCESS; // err code
    va_list argp; // var args, see ex25 for details on va_start, va_arg, va_end
    const char *key = NULL;   // key and value for iterating through the
    const char *arg = NULL;   // args.
    int i = 0;

    // standard apr init stuff, see db.c for discussion
    apr_pool_t *p;
    rv = apr_pool_create(&p, NULL);
    check(rv == APR_SUCCESS, "Failed to create pool.");

    // parse each of the key-value pairs in the ... (every input should be
    // a c string), and use them to fill out entries in the template.
    va_start(argp, template);
    for (key = va_arg(argp, const char *);
         key != NULL;  // note a NULL is used to terminate the arg list
         key = va_arg(argp, const char *)) {

        // we got the key; now get the value
        // ... in the Shells below, the values are always in all-caps.
        arg = va_arg(argp, const char *);

        // then, do a lookup of the key in the template, and replace it
        // with the value if we find a match. Note that this loop explains why
        // the .args all end in NULL: it lets us know when we have hit the
        // end.
        //   We don't raise an error if we fail to find a match (probably
        // should)
        //   We modify the template in-place. That's okay because they get
        // passed in by value, not by reference (the string pointers are all
        // references, but we don't modify the data, just the pointers).
        for (i = 0; template.args[i] != NULL; i++) {
            // oddly given Zed's 
            if (strcmp(template.args[i], key) == 0) {
                template.args[i] = arg;
                break;
            }
        }
    }

    // now we've filled out args and intialized p; run the Shell!
    rc = Shell_run(p, &template);
    va_end(argp); // this goes before the error because only the check
                  // of rv can produce a goto error.

error: // fallthrough
    if (p) apr_pool_destroy(p);
    return rc;
}

/* There are a lot of apr proc-related functions here.
 *   the best docs page I found for this material is here:
 *   http://apr.apache.org/docs/apr/1.3/group__apr__thread__proc.html
 * This funcition creates a new subprocess to run a shell command. It
 * checks both the exit code and that it exited normally from apr's point of
 * view, and returns -1 if either of these show issues. Otherwise it returns 0.
 */
int Shell_run(apr_pool_t *p, Shell *cmd) {
    apr_procattr_t *attr;
    apr_status_t rv;
    apr_proc_t newproc;

    // Create the procattr_t and initialize it. It lives in `p`s memory space.
    rv = apr_procattr_create(&attr, p);
    check(rv == APR_SUCCESS, "Failed to create proc attr.");

    // set stdin, stdout, stderr for the process (in that order).
    //   The inputs are of time apr_int_32_t; I'm guessing they correspond
    //   to file descriptors in Unix (remember apr is portable!). In this
    //   case we are telling apr to inherit them from this process.
    rv = apr_procattr_io_set(attr, APR_NO_PIPE, APR_NO_PIPE, APR_NO_PIPE);
    check(rv == APR_SUCCESS, "Failed to create proc attr.");

    // set the process to run in this directory (if we didn't set it, it would
    // inherit from this process, as you might expect).
    rv = apr_procattr_dir_set(attr, cmd->dir);
    check(rv == APR_SUCCESS, "Failed to set root to %s.", cmd->dir);

    // set the command type. There are few options for this you can see in the
    // docs. Here we say that it's a path-based executable, and inherits the
    // env from the current process.
    rv = apr_procattr_cmdtype_set(attr, APR_PROGRAM_PATH);
    check(rv == APR_SUCCESS, "Failed to set cmd type.");

    // create the process itself.
    // - The NULL entry is the ENV table, which would be a list of
    //   '\0'-terminated strings if we weren't using one of the cmdtype's that
    //   inherits the env. If we tried to set it here it would be ignored
    //   anyway.
    // - The first arg in args has to be the program name.
    // - Note that since we are allocating a new apr object, we have to pass p.
    rv = apr_proc_create(&newproc, cmd->exe, cmd->args, NULL, attr, p);
    check(rv == APR_SUCCESS, "Failed to run command.");

    // the command above started the process, this command tells the current
    // program to wait (block) until it finishes. We then store its output
    // in cmd->exit_code and cmd->exit_why.
    //   Note that the & in `&cmd->exit_code` is modifying exit_code, not
    //   cmd. Similarly for exit_why. [personally I'd rather put parens here
    //   and write &(cmd->exit_code), but I guess that's not idomatic]
    // What's the point of having both exit_code and exit_why? Well, I can't
    // give a full answer, but in unix it's easiest to check exit_why if
    // there was an interrupt signal, whereas for a normal run or an
    // intrinsic failure you would want to check exit_code. You would also
    // use exit_why if the process generated a core dump for some reason.
    rv = apr_proc_wait(&newproc, &cmd->exit_code, &cmd->exit_why, APR_WAIT);
    check(rv == APR_SUCCESS, "Failed to run command.");


    check(cmd->exit_code == 0, "%s exited badly.", cmd->exe);
    check(cmd->exit_why == APR_PROC_EXIT, "%s was killed or crashed.",
          cmd->exe);

    return 0;
error:
    return -1;
}

// note: the hard-coded paths should probably have come from another header
// file, since any changes need to be lock-step. But this is a toy project.
Shell CLEANUP_SH = {
    .exe = "rm",
    .dir = "/tmp",
    .args = {"rm", "-rf", "/tmp/pkg-build", "/tmp/pkg-src.tar.gz",
        "/tmp/pkg-src.tar.bz2", "/tmp/DEPENDS", NULL}
};

Shell GIT_SH = {
    .dir = "/tmp",
    .exe = "git",
    .args = {"git", "clone", "URL", "pkg-build", NULL}
};

Shell TAR_SH = {
    .dir = "/tmp/pkg-build",
    .exe = "tar",
    .args = {"tar", "-xzf", "FILE", "--strip-components", "1", NULL}
};

Shell CURL_SH = {
    .dir = "/tmp",
    .exe = "curl",
    .args = {"curl", "-L", "-o", "TARGET", "URL", NULL}
};

Shell CONFIGURE_SH = {
    .exe = "./configure",
    .dir = "/tmp/pkg-build",
    .args = {"configure", "OPTS", NULL},
};

Shell MAKE_SH = {
    .exe = "make",
    .dir = "/tmp/pkg-build",
    .args = {"make", "OPTS", NULL}
};

Shell INSTALL_SH = {
    .exe = "sudo",
    .dir = "/tmp/pkg-build",
    .args = {"sudo", "make", "TARGET", NULL}
};
