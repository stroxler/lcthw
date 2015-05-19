#include <apr_uri.h>
#include <apr_fnmatch.h>
#include <unistd.h>

#include "commands.h"
#include "dbg.h"
#include "bstrlib.h"
#include "db.h"
#include "shell.h"

/* Accepts a filepath, and installs all dependencies.
 *    does not fetch the file, that's done by Command_fetch. */
int Command_depends(apr_pool_t *p, const char *path) {
    FILE *in = NULL;
    bstring line = NULL;

    in = fopen(path, "r");
    check(in != NULL, "Failed to open downloaded depends: %s", path);

    // bgets is like fgets. See db.c for a deeper discussion of what's
    // going on with the cast -- we are casting fgetc to a type bstringlib
    // likes, then using it to do a gets op. The cast is probably illegal in
    // ANSI C, in which case we would need to define a wrapper, but gcc
    // apparently lets us get away with it.
    for (line = bgets((bNgetc)fgetc, in, '\n');
         line != NULL;
         line = bgets((bNgetc)fgetc, in, '\n')) {
        // trim whitewspace, the bstrlib equiv of python's strip
        btrimws(line);
        log_info("Processing depends: %s", bdata(line));
        // note tha dependencies are assumed not to need options
        int rc = Command_install(p, bdata(line), NULL, NULL, NULL);
        check(rc == 0, "Failed to install: %s", bdata(line));
        bdestroy(line);
    }
    fclose(in);
    return 0;
error:
    if (line) bdestroy(line);
    if (in) fclose(in);
    return -1;
}

int Command_fetch(apr_pool_t *p, const char *url, int fetch_only) {
    int rc = 0;
    const char *depends_file = NULL;

    // create the uri as an apr type
    // see https://apr.apache.org/docs/apr-util/0.9/structapr__uri__t.html
    // all you need to know for this program is that:
    //   - port of 0 indicates a default / a port-less url
    //   - path is the url as a string
    //   - scheme is the protocol (e.g. http, ftp, etc). I *think* this allows
    //     us to give plain old file paths if we want to, and then scheme would
    //     be null. The code seems to suggest it, anyway.
    apr_uri_t info = { .port = 0 };
    apr_status_t rv = apr_uri_parse(p, url, &info);
    check(rv = APR_SUCCESS, "Failed to parse URL: %s", url);

    // apr_fnmatch is apr's filename expression matcher. It follows roughly
    // the same rules as filename expansion in bash. The last entry can be
    // used to set flags that modify the pattern matching. 0 leads to defaults.
    //    -> It returns either APR_SUCCESS or APR_FNM_NOMATCH.
    if (apr_fnmatch(GIT_PAT, info.path, 0) == APR_SUCCESS) {
        rc = Shell_exec(GIT_SH, "URL", url, NULL);
        check(rc == 0, "git failed.");
    } else if (apr_fnmatch(DEPEND_PAT, info.path, 0) == APR_SUCCESS) {
        // we want to call Command_depends after downloading.
        check (!fetch_only, "No point in fetching a DEPENDS file");
        // download to depends_file, unless url is a local path.
        if (info.scheme) {
            depends_file = DEPENDS_PATH;
            rc = Shell_exec(CURL_SH, "URL", url, "TARGET", depends_file, NULL);
            check(rc == 0, "Curl failed.");
        } else {
            depends_file = info.path;
        }
        // recursively process the devpkg list
        log_info("Building according to DEPENDS: %s", url);
        rv = Command_depends(p, depends_file);
        check(rv == 0, "Failed to process the DEPENDS: %s", url);
        // return 0 because we don't actually install anything in *this*
        // command .. although we may have in Command_depends. A return code
        // of 1 means that we still need to build/install, which is not the
        // case here.
        return 0;
    } else if (apr_fnmatch(TAR_GZ_PAT, info.path, 0) == APR_SUCCESS) {
        // download if needed
        if (info.scheme) {
            rc = Shell_exec(CURL_SH, "URL", url, "TARGET", TAR_GZ_SRC, NULL);
            check(rc == 0, "Failed to curl source %s", url);
        }
        // build. See db.c for a discussion of the apr call.
        rv = apr_dir_make_recursive(
                BUILD_DIR, APR_UREAD | APR_UWRITE | APR_UEXECUTE, p
        );
        check(rc == APR_SUCCESS, "Failed to make directory %s", BUILD_DIR);
        rc = Shell_exec(TAR_SH, "FILE", TAR_GZ_SRC, NULL);
        check(rc == 0, "Failed to untar %s", TAR_GZ_SRC);
    } else if (apr_fnmatch(TAR_BZ2_PAT, info.path, 0) == APR_SUCCESS) {
        /* (note: this is copy-paste code - C kinda encourages this sometimes)
         * Zed has a small bug in this code: he redefines rc as apr_status_t
         * and then uses it for the output of Shell_exec. It works, but oops. I
         * fixed it by using copy-pasted code from the preceding block. */
        // download if needed
        if (info.scheme) {
            rc = Shell_exec(CURL_SH, "URL", url, "TARGET", TAR_BZ2_SRC, NULL);
            check(rc == 0, "Failed to curl source %s", url);
        }
        // build. See db.c for a discussion of the apr call.
        rv = apr_dir_make_recursive(
                BUILD_DIR, APR_UREAD | APR_UWRITE | APR_UEXECUTE, p
        );
        check(rc == APR_SUCCESS, "Failed to make directory %s", BUILD_DIR);
        rc = Shell_exec(TAR_SH, "FILE", TAR_BZ2_SRC, NULL);
        check(rc == 0, "Failed to untar %s", TAR_BZ2_SRC);
    } else {
        sentinel("Dont know how to handle this type of url: %s", url);
    }

    return 1; // indicates an install was actually run... the only
              // non-error case where we get a different code is DEPEND.
error:
    return -1;
}

int Command_build(
        apr_pool_t *p, const char *url, const char *configure_opts,
        const char *make_opts, const char *install_opts
) {
    int rc = 0;
    // check that build dir exists
    check(access(BUILD_DIR, X_OK | R_OK | W_OK) == 0,
          "Build directory doesn't exist: %s", BUILD_DIR);
    // if there's a config script, run it.
    if (access(CONFIG_SCRIPT, X_OK) == 0) {
        log_info("Has config script, running it.");
        rc = Shell_exec(CONFIGURE_SH, "OPTS", configure_opts, NULL);
        check(rc == 0, "Failed to configure.");
    }
    // make
    rc = Shell_exec(MAKE_SH, "OPTS", make_opts, NULL);
    check(rc == 0, "Failed to build.");
    // install. Note that if you pass install options, they have to
    // include the work "install", since we only add it if there aren't any lol
    rc = Shell_exec(
            INSTALL_SH, "TARGET", install_opts? install_opts : "install",
            NULL
    );
    check(rc == 0, "Failed to install.");
    // clean up
    rc = Shell_exec(CLEANUP_SH, NULL);
    check(rc == 0, "Failed to cleanup after build.");
    // update the db
    rc = DB_update(url);
    check(rc == 0, "Failed to add this package to the database.");

    return 0;
error:
    return -1;
}

int Command_install(
        apr_pool_t *p, const char *url, const char *configure_opts,
        const char *make_opts, const char *install_opts
) {
    int rc = 0;
    // first clean up
    check(Shell_exec(CLEANUP_SH, NULL) == 0,
          "Faild to clean up before building.");
    // next, make sure it isn't already installed
    rc = DB_find(url);
    check(rc != -1, "Error checking the installed database");
    if (rc == 1) {
        log_info("Package already installed.");
        return 0;
    }
    // it isn't installed. So, fetch.
    rc = Command_fetch(p, url, 0);
    // if fetch worked, build.
    if (rc == 1) {
        rc = Command_build(p, url, configure_opts, make_opts, install_opts);
    } else if (rc == 0) {
        // no install needed... I haven't read fetch yet so I'm not sure how
        // this would happen.
        log_info("Depends successfully installed: %s", url);
    } else {
        sentinel("Install failed: %s", url);
    }
    // cleanup again. It doesn't matter whether we check errors this time.
    Shell_exec(CLEANUP_SH, NULL);
    return 0;
error:
    return -1;
}
