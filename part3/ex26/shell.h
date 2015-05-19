#ifndef _shell_h
#define _shell_h

#define MAX_COMMAND_ARGS 100

#include <apr_thread_proc.h>

typedef struct Shell {
    // directory to run in
    const char *dir;
    // executable
    const char *exe;
    // note that const char * means we can't modify data, but we can reassign
    // pointers (unlike const char * const)... which we do in Shell_exec.
    // ----
    // The zeroth arg in args is always the same as `exe` up to the `./`.
    // This is built into the way apr does subprocess.
    // ...
    // Note that the pointers are modifiable, and indeed Shell_run modifies
    // them, but the const char ensures that Shell_run won't mangle the data
    // of the original template when it's changing the templates passed by
    // value.
    const char *args[MAX_COMMAND_ARGS];

    apr_exit_why_e exit_why;
    int exit_code;

    // I think the presence of attr and proc here might be a mistake;
    // if you look at Shell_run, you see it defines it's own attr and newproc,
    // and never uses these.
    apr_procattr_t *attr;
    apr_proc_t proc;

} Shell;

int Shell_run(apr_pool_t *p, Shell *cmd);
int Shell_exec(Shell cmd, ...);

extern Shell CLEANUP_SH;
extern Shell GIT_SH;
extern Shell TAR_SH;
extern Shell CURL_SH;
extern Shell CONFIGURE_SH;
extern Shell MAKE_SH;
extern Shell INSTALL_SH;

#endif
